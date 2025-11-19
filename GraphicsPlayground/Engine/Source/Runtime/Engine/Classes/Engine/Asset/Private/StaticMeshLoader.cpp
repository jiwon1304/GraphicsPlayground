#include "ObjLoader.h"

#include <fstream>
#include <sstream>

#include "Core/Container/Set.h"
#include "Core/Container/Map.h"
#include "Engine/UserInterface/Console.h"
#include "AssetLoadResult.h"
#include "Classes/Engine/Asset/StaticMeshAsset.h"

FORCEINLINE uint64 HashIndices(IndexType PosIndex, IndexType UVIndex, IndexType NormalIndex)
{
    static_assert(sizeof(IndexType) * 3 <= sizeof(uint64), "IndexType size too large to fit in uint64 for hashing.");
    return (static_cast<uint64>(PosIndex) << 40)
         | (static_cast<uint64>(UVIndex) << 20)
         | (static_cast<uint64>(NormalIndex) << 0);
}

bool FStaticMeshLoader::LoadStaticMesh(const FFilePath &InFilePath, FObjLoadResult &OutLoadResult)
{
    FFilePath BinaryPath = InFilePath.string() + ".bin";
    if (std::ifstream(BinaryPath).good())
    {
        if (LoadBinary(BinaryPath, OutLoadResult))
        {
            return true;
        }
        else
        {
            // Clear previous load result on failure
            OutLoadResult = FObjLoadResult();
        }
    }

    /**
     * Parse .obj file
     */
    bool Result = LoadObj(InFilePath, OutLoadResult);
    if (!Result)
    {
        UE_LOG(ELogLevel::Error, "FObjLoader::LoadStaticMesh - Failed to load OBJ file: %s", InFilePath.string().c_str());
        return false;
    }

    /**
     * Save to binary for faster load next time
     */
    SaveBinary(BinaryPath, OutLoadResult);
    return true;
}

bool FStaticMeshLoader::LoadObj(const FFilePath& InFilePath, FObjLoadResult& OutLoadResult)
{
    std::ifstream ObjFile(InFilePath);
    if (!ObjFile.is_open())
    {
        return false;
    }

    OutLoadResult.AbsoluteFilePath = InFilePath;
    OutLoadResult.AssetName = InFilePath.stem().string();

    /**
     * Vertex data in .obj file is not interleaved, 
     * so we need to parse positions, uvs, normals separately.
     * Then we create interleaved vertex data.
     */

    /**
     * This struct contains position, normal, uv data but indirectly (using index).
     * We will resolve to actual vertex / index data after parsing.
     */
    struct FFaceVertexIndices
    {
        IndexType PositionIndex;
        IndexType UVIndex;
        IndexType NormalIndex;
    };

    /**
     * Vertex data is duplicated in .obj file.
     * We will remove duplicates after parsing.
     */
    TArray<FVector> DuplicatedPositions;
    TArray<FVector> DuplicatedNormals;
    TArray<FVector2D> DuplicatedUVs;
    
    TArray<FString> MaterialLibraries;

    struct FSubmeshData
    {
        FString ObjectName;
        FString GroupName;
        FString Name;
        FString MaterialName;
        TArray<FFaceVertexIndices> Faces;
    };

    FSubmeshData CurrentSubmesh;
    CurrentSubmesh.ObjectName = "Default";
    CurrentSubmesh.GroupName = "Default";
    CurrentSubmesh.Name = "Default-Default";

    TArray<FSubmeshData> Submeshes;

    std::string Line;
    while (std::getline(ObjFile, Line))
    {
        if (Line.empty() || Line[0] == '#')
        {
            continue;
        }

        std::istringstream LineStream(Line);
        std::string Token;
        LineStream >> Token;

        /**
         * Parsing Vertices
         */
        if (Token == "v") // Position
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            DuplicatedPositions.Add(FVector(X, Y, Z));
            continue;
        }
        else if (Token == "vn") // Normal
        {
            float NormalX, NormalY, NormalZ;
            LineStream >> NormalX >> NormalY >> NormalZ;
            DuplicatedNormals.Add(FVector(NormalX, NormalY, NormalZ));
            continue;
        }
        else if (Token == "vt") // UV
        {
            float U, V;
            LineStream >> U >> V;
            DuplicatedUVs.Add(FVector2D(U, V));
            continue;
        }
        /**
         * Parsing Faces
         */
        else if (Token == "f")
        {
            IndexType FacePositionIndices[4];
            IndexType FaceNormalIndices[4];
            IndexType FaceUVIndices[4];

            uint32 NumParsedVerticesThisFace = 0;

            // Parse each line's face definitions
            while (LineStream >> Token)
            {
                NumParsedVerticesThisFace++;

                std::istringstream TokenStream(Token);
                std::string Part;
                
                IndexType PositionIndex = 0;
                IndexType TextureIndex = INDEXTYPE_MAX;
                IndexType NormalIndex = INDEXTYPE_MAX;
                // v
                if (std::getline(TokenStream, Part, '/'))
                {
                    if (!Part.empty())
                    {
                        PositionIndex = std::stoi(Part) - 1;
                    }
                }

                // vt
                if (std::getline(TokenStream, Part, '/'))
                {
                    if (!Part.empty())
                    {
                        TextureIndex = std::stoi(Part) - 1;
                    }
                }

                // vn
                if (std::getline(TokenStream, Part, '/'))
                {
                    if (!Part.empty())
                    {
                        NormalIndex = std::stoi(Part) - 1;
                    }
                }

                FacePositionIndices[NumParsedVerticesThisFace - 1] = PositionIndex;
                FaceUVIndices[NumParsedVerticesThisFace - 1] = TextureIndex;
                FaceNormalIndices[NumParsedVerticesThisFace - 1] = NormalIndex;
            }

            if (NumParsedVerticesThisFace == 3) // 삼각형
            {
                // 반시계 방향(오른손 좌표계)을 시계 방향(왼손 좌표계)으로 변환: 0-2-1
                CurrentSubmesh.Faces.Add({ FacePositionIndices[0], FaceUVIndices[0], FaceNormalIndices[0] });
                CurrentSubmesh.Faces.Add({ FacePositionIndices[2], FaceUVIndices[2], FaceNormalIndices[2] });
                CurrentSubmesh.Faces.Add({ FacePositionIndices[1], FaceUVIndices[1], FaceNormalIndices[1] });
            }
            else if (NumParsedVerticesThisFace == 4) // 쿼드
            {
                // 첫 번째 삼각형: 0-2-1
                CurrentSubmesh.Faces.Add({ FacePositionIndices[0], FaceUVIndices[0], FaceNormalIndices[0] });
                CurrentSubmesh.Faces.Add({ FacePositionIndices[2], FaceUVIndices[2], FaceNormalIndices[2] });
                CurrentSubmesh.Faces.Add({ FacePositionIndices[1], FaceUVIndices[1], FaceNormalIndices[1] });

                // 두 번째 삼각형: 0-3-2
                CurrentSubmesh.Faces.Add({ FacePositionIndices[0], FaceUVIndices[0], FaceNormalIndices[0] });
                CurrentSubmesh.Faces.Add({ FacePositionIndices[3], FaceUVIndices[3], FaceNormalIndices[3] });
                CurrentSubmesh.Faces.Add({ FacePositionIndices[2], FaceUVIndices[2], FaceNormalIndices[2] });
            }
        }
        // Material declaration
        else if (Token == "mtllib")
        {
            LineStream >> Line;
            MaterialLibraries.Add(Line);
        }
        // Use material for subsequent faces
        else if (Token == "usemtl")
        {
            LineStream >> Line;
            if (!MaterialLibraries.Contains(FString(Line)))
            {
                UE_LOG(ELogLevel::Warning, "Material %s used but not declared in mtllib.", Line.c_str());
            }
            CurrentSubmesh.MaterialName = FString(Line);
        }
        // New submesh for new group or object
        else if (Token == "g" || Token == "o")
        {
            // Cache current group or object due to move semantics for Emplace()
            FString GroupName = CurrentSubmesh.GroupName;
            FString ObjectName = CurrentSubmesh.ObjectName;

            // Save previous submesh data 
            Submeshes.Emplace(CurrentSubmesh);
            // Reset current submesh data
            CurrentSubmesh = FSubmeshData();

            // Update new group / object name
            if (Token == "g")
            {
                // New groupname
                LineStream >> Line;
                CurrentSubmesh.GroupName = FString(Line);
                CurrentSubmesh.ObjectName = ObjectName; // keep previous object name
            }
            else
            {
                // New objectname
                LineStream >> Line;
                CurrentSubmesh.ObjectName = FString(Line);
                CurrentSubmesh.GroupName = GroupName; // keep previous group name
            }
            // Update submesh name
            CurrentSubmesh.Name = CurrentSubmesh.ObjectName + "-" + CurrentSubmesh.GroupName;
        }
    }
    // save last submesh (if no explicit group/object appeared, this keeps the default one)
    if (!CurrentSubmesh.Faces.IsEmpty())
    {
        Submeshes.Emplace(CurrentSubmesh);
    }

    // Prepare output containers
    OutLoadResult.Vertices.Empty();
    OutLoadResult.SubMeshes.Empty();
    OutLoadResult.PositionOnly.Empty();
    OutLoadResult.PositionOnlyIndices.Empty();

    // Estimate counts to minimize reallocations
    int32 TotalFaceVertices = 0;
    for (const FSubmeshData& Submesh : Submeshes)
    {
        TotalFaceVertices += Submesh.Faces.Num();
    }
    if (TotalFaceVertices > INDEXTYPE_MAX)
    {
        UE_LOG(ELogLevel::Warning, "Number of indices exceeds uint16 when loading %s", InFilePath.string().c_str());
    }

    OutLoadResult.Vertices.Reserve(TotalFaceVertices);
    TArray<IndexType> IndexBuffer;
    IndexBuffer.Reserve(TotalFaceVertices);

    TMap<uint64, IndexType> UniqueVertexRemap;
    UniqueVertexRemap.Reserve(TotalFaceVertices);

    // Helper lambdas to safely fetch attributes
    auto GetPosition = [&](IndexType Index) -> const FVector&
    {
        if (Index < 0 || Index >= DuplicatedPositions.Num())
        {
            static const FVector ZeroVector(0.f, 0.f, 0.f);
            UE_LOG(ELogLevel::Warning, "Invalid position index %d while loading %s", Index, InFilePath.string().c_str());
            return ZeroVector;
        }
        return DuplicatedPositions[Index];
    };

    auto GetNormal = [&](IndexType Index) -> FVector
    {
        if (Index == INDEXTYPE_MAX || Index < 0 || Index >= DuplicatedNormals.Num())
        {
            return FVector(0.f, 0.f, 1.f);
        }
        return DuplicatedNormals[Index];
    };

    auto GetUV = [&](IndexType Index) -> FVector2D
    {
        if (Index == INDEXTYPE_MAX || Index < 0 || Index >= DuplicatedUVs.Num())
        {
            return FVector2D(0.f, 0.f);
        }
        return DuplicatedUVs[Index];
    };

    const FFilePath ObjDirectory = InFilePath.parent_path();
    const bool bHasMaterialLibrary = !MaterialLibraries.IsEmpty();

    for (const FSubmeshData& Submesh : Submeshes)
    {
        if (Submesh.Faces.IsEmpty())
        {
            continue;
        }

        FObjLoadResult::FSubMeshInfo SubmeshInfo{};
        SubmeshInfo.IndexStart = static_cast<IndexType>(IndexBuffer.Num());
        SubmeshInfo.IndexCount = 0;

        SubmeshInfo.MaterialPath = FFilePath(ObjDirectory.string() + "/" + Submesh.MaterialName.ToUTF8String());

        for (const FFaceVertexIndices& FaceVertex : Submesh.Faces)
        {
            if (FaceVertex.PositionIndex < 0 || FaceVertex.PositionIndex >= DuplicatedPositions.Num())
            {
                UE_LOG(ELogLevel::Warning, "Skipping invalid face vertex (pos=%d) in %s", FaceVertex.PositionIndex, InFilePath.string().c_str());
                continue;
            }

            const IndexType SafeUVIndex = (FaceVertex.UVIndex == INDEXTYPE_MAX) ? INDEXTYPE_MAX : FaceVertex.UVIndex;
            const IndexType SafeNormalIndex = (FaceVertex.NormalIndex == INDEXTYPE_MAX) ? INDEXTYPE_MAX : FaceVertex.NormalIndex;
            const uint64 VertexKey = HashIndices(FaceVertex.PositionIndex, SafeUVIndex, SafeNormalIndex);

            IndexType* ExistingIndex = UniqueVertexRemap.Find(VertexKey);
            IndexType FinalVertexIndex;

            if (ExistingIndex)
            {
                FinalVertexIndex = *ExistingIndex;
            }
            else
            {
                FinalVertexIndex = static_cast<IndexType>(OutLoadResult.Vertices.Num());

                FStaticMeshVertex Vertex{};
                const FVector& Position = GetPosition(FaceVertex.PositionIndex);
                Vertex.X = Position.X;
                Vertex.Y = Position.Y;
                Vertex.Z = Position.Z;
                Vertex.R = DefaultColor[0];
                Vertex.G = DefaultColor[1];
                Vertex.B = DefaultColor[2];
                Vertex.A = DefaultColor[3];

                const FVector Normal = GetNormal(SafeNormalIndex);
                Vertex.NormalX = Normal.X;
                Vertex.NormalY = Normal.Y;
                Vertex.NormalZ = Normal.Z;
                Vertex.TangentX = 0.f;
                Vertex.TangentY = 0.f;
                Vertex.TangentZ = 0.f;
                Vertex.TangentW = 1.f;

                const FVector2D UV = GetUV(SafeUVIndex);
                Vertex.U = UV.X;
                Vertex.V = UV.Y;

                Vertex.MaterialIndex = 0; // Will be mapped when materials are resolved

                UniqueVertexRemap.Add(VertexKey, FinalVertexIndex);
                OutLoadResult.Vertices.Add(Vertex);
            }

            IndexBuffer.Add(FinalVertexIndex);
            ++SubmeshInfo.IndexCount;
        }

        if (SubmeshInfo.IndexCount > 0)
        {
            OutLoadResult.SubMeshes.Add(SubmeshInfo);
        }
    }

    if (!IndexBuffer.IsEmpty())
    {
        if (IndexBuffer.Num() % 3 != 0)
        {
            UE_LOG(ELogLevel::Warning, "Index buffer is not divisible by 3 when loading %s", InFilePath.string().c_str());
        }

        for (int32 Idx = 0; Idx + 2 < IndexBuffer.Num(); Idx += 3)
        {
            const IndexType I0 = IndexBuffer[Idx];
            const IndexType I1 = IndexBuffer[Idx + 1];
            const IndexType I2 = IndexBuffer[Idx + 2];

            if (I0 >= OutLoadResult.Vertices.Num() || I1 >= OutLoadResult.Vertices.Num() || I2 >= OutLoadResult.Vertices.Num())
            {
                UE_LOG(ELogLevel::Warning, "Invalid index while calculating tangents in %s", InFilePath.string().c_str());
                continue;
            }

            CalculateTangentForPivot(OutLoadResult.Vertices[I0], OutLoadResult.Vertices[I1], OutLoadResult.Vertices[I2]);
            CalculateTangentForPivot(OutLoadResult.Vertices[I1], OutLoadResult.Vertices[I2], OutLoadResult.Vertices[I0]);
            CalculateTangentForPivot(OutLoadResult.Vertices[I2], OutLoadResult.Vertices[I0], OutLoadResult.Vertices[I1]);
        }
    }

    // Store index buffer and position-only stream derived from unique vertices
    OutLoadResult.PositionOnlyIndices = std::move(IndexBuffer);
    OutLoadResult.PositionOnly.Reserve(OutLoadResult.Vertices.Num());
    for (const FStaticMeshVertex& Vertex : OutLoadResult.Vertices)
    {
        OutLoadResult.PositionOnly.Add(FVector(Vertex.X, Vertex.Y, Vertex.Z));
    }

    return true;
}

bool FStaticMeshLoader::SaveBinary(const FFilePath &BinaryPath, const FObjLoadResult &InStaticMesh)
{
    std::ofstream File(BinaryPath, std::ios::binary);
    if (!File.is_open())
    {
        UE_LOG(ELogLevel::Error, "FStaticMeshLoader::SaveBinary - Can't open file for writing: %s", BinaryPath.string().c_str());
        return false;
    }

    Serializer::WriteFString(File, InStaticMesh.AssetName);

    uint32 PoisitonOnlyCount = InStaticMesh.PositionOnly.Num();
    File.write(reinterpret_cast<const char*>(&PoisitonOnlyCount), sizeof(PoisitonOnlyCount));
    File.write(reinterpret_cast<const char*>(InStaticMesh.PositionOnly.GetData()), PoisitonOnlyCount * sizeof(InStaticMesh.PositionOnly[0]));

    uint32 PositionOnlyIndexCount = InStaticMesh.PositionOnlyIndices.Num();
    File.write(reinterpret_cast<const char*>(&PositionOnlyIndexCount), sizeof(PositionOnlyIndexCount));
    File.write(reinterpret_cast<const char*>(InStaticMesh.PositionOnlyIndices.GetData()), PositionOnlyIndexCount * sizeof(InStaticMesh.PositionOnlyIndices[0]));
    
    uint32 VertexCount = InStaticMesh.Vertices.Num();
    File.write(reinterpret_cast<const char*>(&VertexCount), sizeof(VertexCount));
    File.write(reinterpret_cast<const char*>(InStaticMesh.Vertices.GetData()), VertexCount * sizeof(InStaticMesh.Vertices[0]));

    uint32 SubmeshCount = InStaticMesh.SubMeshes.Num();
    File.write(reinterpret_cast<const char*>(&SubmeshCount), sizeof(SubmeshCount));
    for (const FObjLoadResult::FSubMeshInfo& Submesh : InStaticMesh.SubMeshes)
    {
        File.write(reinterpret_cast<const char*>(&Submesh.IndexStart), sizeof(Submesh.IndexStart));
        File.write(reinterpret_cast<const char*>(&Submesh.IndexCount), sizeof(Submesh.IndexCount));
        Serializer::WriteFString(File, Submesh.MaterialPath.string());
    }
}

bool FStaticMeshLoader::LoadBinary(const FFilePath &BinaryPath, FObjLoadResult &OutStaticMesh)
{
    std::ifstream File(BinaryPath, std::ios::binary);
    if (!File.is_open())
    {
        UE_LOG(ELogLevel::Error, "FStaticMeshLoader::LoadBinary - Can't open file for reading: %s", BinaryPath.string().c_str());
        return false;
    }

    Serializer::ReadFString(File, OutStaticMesh.AssetName);

    uint32 PoisitonOnlyCount = 0;
    File.read(reinterpret_cast<char*>(&PoisitonOnlyCount), sizeof(PoisitonOnlyCount));
    OutStaticMesh.PositionOnly.SetNum(PoisitonOnlyCount);
    File.read(reinterpret_cast<char*>(OutStaticMesh.PositionOnly.GetData()), PoisitonOnlyCount * sizeof(OutStaticMesh.PositionOnly[0]));

    uint32 PositionOnlyIndexCount = 0;
    File.read(reinterpret_cast<char*>(&PositionOnlyIndexCount), sizeof(PositionOnlyIndexCount));
    OutStaticMesh.PositionOnlyIndices.SetNum(PositionOnlyIndexCount);
    File.read(reinterpret_cast<char*>(OutStaticMesh.PositionOnlyIndices.GetData()), PositionOnlyIndexCount * sizeof(OutStaticMesh.PositionOnlyIndices[0]));

    uint32 VertexCount = 0;
    File.read(reinterpret_cast<char*>(&VertexCount), sizeof(VertexCount));
    OutStaticMesh.Vertices.SetNum(VertexCount);
    File.read(reinterpret_cast<char*>(OutStaticMesh.Vertices.GetData()), VertexCount * sizeof(OutStaticMesh.Vertices[0]));

    uint32 SubmeshCount = 0;
    File.read(reinterpret_cast<char*>(&SubmeshCount), sizeof(SubmeshCount));
    OutStaticMesh.SubMeshes.SetNum(SubmeshCount);
    for (FObjLoadResult::FSubMeshInfo& Submesh : OutStaticMesh.SubMeshes)
    {
        File.read(reinterpret_cast<char*>(&Submesh.IndexStart), sizeof(Submesh.IndexStart));
        File.read(reinterpret_cast<char*>(&Submesh.IndexCount), sizeof(Submesh.IndexCount));
        FString MaterialPath;
        Serializer::ReadFString(File, MaterialPath);
        Submesh.MaterialPath = MaterialPath.ToUTF8String();
    }

    return true;
}

void FStaticMeshLoader::CalculateTangentForPivot(FStaticMeshVertex &PivotVertex, const FStaticMeshVertex &Vertex1, const FStaticMeshVertex &Vertex2)
{
    const float s1 = Vertex1.U - PivotVertex.U;
    const float t1 = Vertex1.V - PivotVertex.V;
    const float s2 = Vertex2.U - PivotVertex.U;
    const float t2 = Vertex2.V - PivotVertex.V;
    const float E1x = Vertex1.X - PivotVertex.X;
    const float E1y = Vertex1.Y - PivotVertex.Y;
    const float E1z = Vertex1.Z - PivotVertex.Z;
    const float E2x = Vertex2.X - PivotVertex.X;
    const float E2y = Vertex2.Y - PivotVertex.Y;
    const float E2z = Vertex2.Z - PivotVertex.Z;

    const float Denominator = s1 * t2 - s2 * t1;
    FVector Tangent(1.f, 0.f, 0.f);
    FVector BiTangent(0.f, 1.f, 0.f);
    FVector Normal(PivotVertex.NormalX, PivotVertex.NormalY, PivotVertex.NormalZ);

    if (FMath::Abs(Denominator) > SMALL_NUMBER)
    {
        const float f = 1.f / Denominator;

        const float Tx = f * (t2 * E1x - t1 * E2x);
        const float Ty = f * (t2 * E1y - t1 * E2y);
        const float Tz = f * (t2 * E1z - t1 * E2z);
        Tangent = FVector(Tx, Ty, Tz).GetSafeNormal();

        const float Bx = f * (-s2 * E1x + s1 * E2x);
        const float By = f * (-s2 * E1y + s1 * E2y);
        const float Bz = f * (-s2 * E1z + s1 * E2z);
        BiTangent = FVector(Bx, By, Bz).GetSafeNormal();
    }
    else
    {
        FVector Edge1(E1x, E1y, E1z);
        FVector Edge2(E2x, E2y, E2z);
        Tangent = (Edge1.GetSafeNormal() + Edge2.GetSafeNormal()).GetSafeNormal();
        if (Tangent.IsNearlyZero())
        {
            Tangent = FVector(1.0f, 0.0f, 0.0f);
        }
    }

    Tangent = (Tangent - Normal * FVector::DotProduct(Normal, Tangent)).GetSafeNormal();
    const float Sign = (FVector::DotProduct(FVector::CrossProduct(Normal, Tangent), BiTangent) < 0.f) ? -1.f : 1.f;

    PivotVertex.TangentX = Tangent.X;
    PivotVertex.TangentY = Tangent.Y;
    PivotVertex.TangentZ = Tangent.Z;
    PivotVertex.TangentW = Sign;
}



// bool FObjLoader::SaveStaticMeshToBinary(const FWString& FilePath, const FStaticMeshRenderData& StaticMesh)
// {
//     std::ofstream File(FilePath, std::ios::binary);
//     if (!File.is_open())
//     {
//         assert("CAN'T SAVE STATIC MESH BINARY FILE");
//         return false;
//     }

//     // Object Name
//     Serializer::WriteFWString(File, StaticMesh.ObjectName);

//     // Display Name
//     Serializer::WriteFString(File, StaticMesh.DisplayName);

//     // Vertices
//     uint32 VertexCount = StaticMesh.Vertices.Num();
//     File.write(reinterpret_cast<const char*>(&VertexCount), sizeof(VertexCount));
//     File.write(reinterpret_cast<const char*>(StaticMesh.Vertices.GetData()), VertexCount * sizeof(StaticMesh.Vertices[0]));

//     // Indices
//     uint32 IndexCount = StaticMesh.Indices.Num();
//     File.write(reinterpret_cast<const char*>(&IndexCount), sizeof(IndexCount));
//     File.write(reinterpret_cast<const char*>(StaticMesh.Indices.GetData()), IndexCount * sizeof(StaticMesh.Indices[0]));

//     // Materials
//     uint32 MaterialCount = StaticMesh.Materials.Num();
//     File.write(reinterpret_cast<const char*>(&MaterialCount), sizeof(MaterialCount));
//     for (const FMaterialInfo* Material : StaticMesh.Materials)
//     {
//         Serializer::WriteFString(File, Material->MaterialName);
//         Serializer::WriteFWString(File, Material->MaterialPath);
        
//         File.write(reinterpret_cast<const char*>(&Material->TextureFlag), sizeof(Material->TextureFlag));
        
//         File.write(reinterpret_cast<const char*>(&Material->bTransparent), sizeof(Material->bTransparent));
//         File.write(reinterpret_cast<const char*>(&Material->DiffuseColor), sizeof(Material->DiffuseColor));
//         File.write(reinterpret_cast<const char*>(&Material->SpecularColor), sizeof(Material->SpecularColor));
//         File.write(reinterpret_cast<const char*>(&Material->AmbientColor), sizeof(Material->AmbientColor));
//         File.write(reinterpret_cast<const char*>(&Material->EmissiveColor), sizeof(Material->EmissiveColor));
        
//         File.write(reinterpret_cast<const char*>(&Material->Shininess), sizeof(Material->Shininess));
//         File.write(reinterpret_cast<const char*>(&Material->IOR), sizeof(Material->IOR));
//         File.write(reinterpret_cast<const char*>(&Material->Transparency), sizeof(Material->Transparency));
//         File.write(reinterpret_cast<const char*>(&Material->BumpMultiplier), sizeof(Material->BumpMultiplier));
//         File.write(reinterpret_cast<const char*>(&Material->IlluminanceModel), sizeof(Material->IlluminanceModel));

//         File.write(reinterpret_cast<const char*>(&Material->Metallic), sizeof(Material->Metallic));
//         File.write(reinterpret_cast<const char*>(&Material->Roughness), sizeof(Material->Roughness));

//         for (uint8 i = 0; i < static_cast<uint8>(EMaterialTextureSlots::MTS_MAX); ++i)
//         {
//             Serializer::WriteFString(File, Material->TextureInfos[i].TextureName);
//             Serializer::WriteFWString(File, Material->TextureInfos[i].TexturePath);
//             File.write(reinterpret_cast<const char*>(&Material->TextureInfos[i].bIsSRGB), sizeof(Material->TextureInfos[i].bIsSRGB));
//         }
//     }

//     // Material Subsets
//     uint32 SubsetCount = StaticMesh.MaterialSubsets.Num();
//     File.write(reinterpret_cast<const char*>(&SubsetCount), sizeof(SubsetCount));
//     for (const FMaterialSubset& Subset : StaticMesh.MaterialSubsets)
//     {
//         Serializer::WriteFString(File, Subset.MaterialName);
//         File.write(reinterpret_cast<const char*>(&Subset.IndexStart), sizeof(Subset.IndexStart));
//         File.write(reinterpret_cast<const char*>(&Subset.IndexCount), sizeof(Subset.IndexCount));
//         File.write(reinterpret_cast<const char*>(&Subset.MaterialIndex), sizeof(Subset.MaterialIndex));
//     }

//     // Bounding Box
//     File.write(reinterpret_cast<const char*>(&StaticMesh.BoundingBoxMin), sizeof(StaticMesh.BoundingBoxMin[0]));
//     File.write(reinterpret_cast<const char*>(&StaticMesh.BoundingBoxMax), sizeof(StaticMesh.BoundingBoxMax[0]));

//     File.close();
//     return true;
// }

// FStaticMeshRenderData* FObjLoader::LoadStaticMeshFromBinary(const FWString& FilePath)
// {
//     std::ifstream File(FilePath, std::ios::binary);
//     if (!File.is_open())
//     {
//         UE_LOG(ELogLevel::Error, "Failed to load static mesh from binary: %s", FilePath.c_str());
//         return nullptr;
//     }

//     TArray<TPair<FWString, bool>> Textures;

//     FStaticMeshRenderData* OutStaticMesh = new FStaticMeshRenderData();

//     // Object Name
//     Serializer::ReadFWString(File, OutStaticMesh->ObjectName);

//     //// Path Name
//     //Serializer::ReadFWString(File, OutStaticMesh->PathName);

//     // Display Name
//     Serializer::ReadFString(File, OutStaticMesh->DisplayName);

//     // Vertices
//     uint32 VertexCount = 0;
//     File.read(reinterpret_cast<char*>(&VertexCount), sizeof(VertexCount));
//     OutStaticMesh->Vertices.SetNum(VertexCount);
//     File.read(reinterpret_cast<char*>(OutStaticMesh->Vertices.GetData()), VertexCount * sizeof(OutStaticMesh->Vertices[0]));

//     // Indices
//     uint32 IndexCount = 0;
//     File.read(reinterpret_cast<char*>(&IndexCount), sizeof(IndexCount));
//     OutStaticMesh->Indices.SetNum(IndexCount);
//     File.read(reinterpret_cast<char*>(OutStaticMesh->Indices.GetData()), IndexCount * sizeof(OutStaticMesh->Indices[0]));

//     // Material
//     uint32 MaterialCount = 0;
//     File.read(reinterpret_cast<char*>(&MaterialCount), sizeof(MaterialCount));
//     OutStaticMesh->Materials.SetNum(MaterialCount);
//     for (FMaterialInfo* Material : OutStaticMesh->Materials)
//     {
//         Serializer::ReadFString(File, Material->MaterialName);
//         Serializer::ReadFWString(File, Material->MaterialPath);

//         File.read(reinterpret_cast<char*>(&Material->TextureFlag), sizeof(Material->TextureFlag));
        
//         File.read(reinterpret_cast<char*>(&Material->bTransparent), sizeof(Material->bTransparent));
//         File.read(reinterpret_cast<char*>(&Material->DiffuseColor), sizeof(Material->DiffuseColor));
//         File.read(reinterpret_cast<char*>(&Material->SpecularColor), sizeof(Material->SpecularColor));
//         File.read(reinterpret_cast<char*>(&Material->AmbientColor), sizeof(Material->AmbientColor));
//         File.read(reinterpret_cast<char*>(&Material->EmissiveColor), sizeof(Material->EmissiveColor));
        
//         File.read(reinterpret_cast<char*>(&Material->Shininess), sizeof(Material->Shininess));
//         File.read(reinterpret_cast<char*>(&Material->IOR), sizeof(Material->IOR));
//         File.read(reinterpret_cast<char*>(&Material->Transparency), sizeof(Material->Transparency));
//         File.read(reinterpret_cast<char*>(&Material->BumpMultiplier), sizeof(Material->BumpMultiplier));
//         File.read(reinterpret_cast<char*>(&Material->IlluminanceModel), sizeof(Material->IlluminanceModel));

//         File.read(reinterpret_cast<char*>(&Material->Metallic), sizeof(Material->Metallic));
//         File.read(reinterpret_cast<char*>(&Material->Roughness), sizeof(Material->Roughness));

//         uint8 TextureNum = static_cast<uint8>(EMaterialTextureSlots::MTS_MAX);
//         Material->TextureInfos.SetNum(TextureNum);
//         for (uint8 i = 0; i < TextureNum; ++i)
//         {
//             Serializer::ReadFString(File, Material->TextureInfos[i].TextureName);
//             Serializer::ReadFWString(File, Material->TextureInfos[i].TexturePath);
//             File.read(reinterpret_cast<char*>(&Material->TextureInfos[i].bIsSRGB), sizeof(Material->TextureInfos[i].bIsSRGB));

//             Textures.AddUnique({Material->TextureInfos[i].TexturePath, Material->TextureInfos[i].bIsSRGB});
//         }
//     }

//     // Material Subset
//     uint32 SubsetCount = 0;
//     File.read(reinterpret_cast<char*>(&SubsetCount), sizeof(SubsetCount));
//     OutStaticMesh->MaterialSubsets.SetNum(SubsetCount);
//     for (FMaterialSubset& Subset : OutStaticMesh->MaterialSubsets)
//     {
//         Serializer::ReadFString(File, Subset.MaterialName);
//         File.read(reinterpret_cast<char*>(&Subset.IndexStart), sizeof(Subset.IndexStart));
//         File.read(reinterpret_cast<char*>(&Subset.IndexCount), sizeof(Subset.IndexCount));
//         File.read(reinterpret_cast<char*>(&Subset.MaterialIndex), sizeof(Subset.MaterialIndex));
//     }

//     // Bounding Box
//     File.read(reinterpret_cast<char*>(&OutStaticMesh->BoundingBoxMin), sizeof(OutStaticMesh->BoundingBoxMin[0]));
//     File.read(reinterpret_cast<char*>(&OutStaticMesh->BoundingBoxMax), sizeof(OutStaticMesh->BoundingBoxMax[0]));

//     File.close();

//     // Texture Load
//     if (Textures.Num() > 0)
//     {
//         for (const TPair<FWString, bool>& Texture : Textures)
//         {
//             if (GEngineLoop.ResourceManager->GetTexture(Texture.Key) == nullptr)
//             {
//                 GEngineLoop.ResourceManager->LoadTextureFromFile(/* GEngineLoop.GraphicDevice->Device,  */Texture.Key.c_str(), Texture.Value);
//             }
//         }
//     }

//     return OutStaticMesh;
// }

//
//UMaterial* FObjLoader::CreateMaterial(const FMaterialInfo& MaterialInfo)
//{
//    if (MaterialMap[MaterialInfo.MaterialName] != nullptr)
//    {
//        return MaterialMap[MaterialInfo.MaterialName];
//    }
//
//    UMaterial* NewMaterial = FObjectFactory::ConstructObject<UMaterial>(nullptr); // Material은 Outer가 없이 따로 관리되는 객체이므로 Outer가 없음으로 설정. 추후 Garbage Collection이 추가되면 AssetManager를 생성해서 관리.
//    NewMaterial->SetMaterialInfo(MaterialInfo);
//    MaterialMap.Add(MaterialInfo.MaterialName, NewMaterial);
//    return NewMaterial;
//}
//
//UMaterial* FObjManager::GetMaterial(const FString& Name)
//{
//    return MaterialMap[Name];
//}

// UStaticMesh* FObjManager::CreateStaticMesh(const FString& FilePath)
// {
//     FStaticMeshRenderData* StaticMeshRenderData = FObjManager::LoadObjStaticMeshAsset(FilePath);

//     if (StaticMeshRenderData == nullptr)
//     {
//         return nullptr;
//     }

//     UStaticMesh* StaticMesh = GetStaticMesh(StaticMeshRenderData->ObjectName);
//     if (StaticMesh != nullptr)
//     {
//         return StaticMesh;
//     }

//     StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>(nullptr); // TODO: 추후 AssetManager를 생성해서 관리.
//     StaticMesh->SetData(StaticMeshRenderData);

//     // 임시 BodySetup 붙여주기
//     UBodySetup* BodySetup = FObjectFactory::ConstructObject<UBodySetup>(StaticMesh);
//     BodySetup->CollisionResponse = EBodyCollisionResponse::Type::BodyCollision_Enabled;
//     BodySetup->DefaultInstance.ExternalCollisionProfileBodySetup = BodySetup;

// 	FPhysicsMaterial* PhysicsMaterial = new FPhysicsMaterial();
// 	PhysicsMaterial->Density = 1000.f; // 임시 밀도 값

// 	UPhysicalMaterial* PhysMaterial = FObjectFactory::ConstructObject<UPhysicalMaterial>(nullptr);
// 	PhysMaterial->Material = PhysicsMaterial;
// 	FKBoxElem BoxElem;

// 	BodySetup->AggGeom.BoxElems.Add(BoxElem);
// 	BodySetup->PhysMaterial = PhysMaterial;

// 	StaticMesh->SetBodySetup(BodySetup);

//     StaticMeshMap.Add(StaticMeshRenderData->ObjectName, StaticMesh); // TODO: 장기적으로 보면 파일 이름 대신 경로를 Key로 사용하는게 좋음.
//     return StaticMesh;
// }

// //UStaticMesh* FObjManager::GetStaticMesh(const FWString& Name)
// //{
// //    return StaticMeshMap[Name];
// //}
