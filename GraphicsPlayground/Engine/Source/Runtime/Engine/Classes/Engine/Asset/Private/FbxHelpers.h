#include <fbxsdk.h>

#include "AssetLoadData.h"

FTransform ConvertFbxTransformToFTransform(FbxNode* Node)
{
    FbxAMatrix LocalMatrix = Node->EvaluateLocalTransform();

    // FBX 행렬에서 스케일, 회전, 위치 추출
    FbxVector4 T = LocalMatrix.GetT();
    FbxVector4 S = LocalMatrix.GetS();
    FbxQuaternion Q = LocalMatrix.GetQ();
    
    // 언리얼 엔진 형식으로 변환
    FVector Translation(
        static_cast<float>(T[0]),
        static_cast<float>(T[1]),
        static_cast<float>(T[2])
    );
    
    FVector Scale(
        static_cast<float>(S[0]),
        static_cast<float>(S[1]),
        static_cast<float>(S[2])
    );
    
    FQuat Rotation(
        static_cast<float>(Q[0]),
        static_cast<float>(Q[1]),
        static_cast<float>(Q[2]),
        static_cast<float>(Q[3])
    );
    Rotation.Normalize();
    
    return FTransform(Rotation, Translation, Scale);
}

FMatrix ConvertFbxMatrixToFMatrix(const FbxAMatrix& FbxMatrix)
{
    FMatrix Result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Result.M[i][j] = static_cast<float>(FbxMatrix.Get(i, j));
        }
    }
    return Result;
}

FbxAMatrix ConvertFbxMatrixToFbxAMatrix(const FbxMatrix& Matrix)
{
    FbxVector4    T, S, Shear;
    FbxQuaternion Q;
    double        Sign;
    Matrix.GetElements(T, Q, Shear, S, Sign);   // GetElements(translation, quaternion, shearing, scale, sign)

    FbxAMatrix Result;
    Result.SetTQS(T, Q, S);
    return Result;
    
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            Result[r][c] = Matrix.Get(r, c);
        }
    }
    return Result;
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 XYZ로 변환 (좌표계 변환 포함)
template<typename T>
void SetVertexPosition(T& Vertex, const FbxVector4& Pos)
{
    Vertex.X = static_cast<float>(Pos[0]);
    Vertex.Y = static_cast<float>(Pos[1]);
    Vertex.Z = static_cast<float>(Pos[2]);
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 Normal XYZ로 변환 (좌표계 변환 포함)
template<typename T>
void SetVertexNormal(T& Vertex, const FbxVector4& Normal)
{
    Vertex.NormalX = static_cast<float>(Normal[0]);
    Vertex.NormalY = static_cast<float>(Normal[1]);
    Vertex.NormalZ = static_cast<float>(Normal[2]);
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 Tangent XYZW로 변환 (좌표계 변환 포함)
template<typename T>
void SetVertexTangent(T& Vertex, const FbxVector4& Tangent)
{
    Vertex.TangentX = static_cast<float>(Tangent[0]);
    Vertex.TangentY = static_cast<float>(Tangent[1]);
    Vertex.TangentZ = static_cast<float>(Tangent[2]);
    Vertex.TangentW = static_cast<float>(Tangent[3]); // W (Handedness)
}

// 헬퍼 함수: FbxColor를 FSkeletalMeshVertex의 RGBA로 변환
template<typename T>
void SetVertexColor(T& Vertex, const FbxColor& Color)
{
    Vertex.R = static_cast<float>(Color.mRed);
    Vertex.G = static_cast<float>(Color.mGreen);
    Vertex.B = static_cast<float>(Color.mBlue);
    Vertex.A = static_cast<float>(Color.mAlpha);
}

// 헬퍼 함수: FbxVector2를 FSkeletalMeshVertex의 UV로 변환 (좌표계 변환 포함)
template<typename T>
void SetVertexUV(T& Vertex, const FbxVector2& UV)
{
    Vertex.U = static_cast<float>(UV[0]);
    Vertex.V = 1.0f - static_cast<float>(UV[1]); // V 좌표는 보통 뒤집힘 (DirectX 스타일)
}

void CalculateTangent_Internal(FStaticMeshVertex& PivotVertex, const FStaticMeshVertex& Vertex1, const FStaticMeshVertex& Vertex2)
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
    FVector Tangent(1, 0, 0);
    FVector BiTangent(0, 1, 0);
    FVector Normal(PivotVertex.NormalX, PivotVertex.NormalY, PivotVertex.NormalZ);
    
    if (FMath::Abs(Denominator) > SMALL_NUMBER)
    {
        // 정상적인 계산 진행
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
        // 대체 탄젠트 계산 방법
        // 방법 1: 다른 방향에서 탄젠트 계산 시도
        FVector Edge1(E1x, E1y, E1z);
        FVector Edge2(E2x, E2y, E2z);
    
        // 기하학적 접근: 두 에지 사이의 각도 이등분선 사용
        Tangent = (Edge1.GetSafeNormal() + Edge2.GetSafeNormal()).GetSafeNormal();
    
        // 만약 두 에지가 평행하거나 반대 방향이면 다른 방법 사용
        if (Tangent.IsNearlyZero())
        {
            // TODO: 기본 축 방향 중 하나 선택 (메시의 주 방향에 따라 선택)
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

template <typename T>
void CalculateTangents(TArray<T>& Vertices, const TArray<IndexType>& Indices)
{
    // 탄젠트 초기화
    for (T& Vertex : Vertices)
    {
        Vertex.TangentX = 0.0f;
        Vertex.TangentY = 0.0f;
        Vertex.TangentZ = 0.0f;
        Vertex.TangentW = 0.0f;
    }

    // 각 삼각형마다 탄젠트 계산
    for (int32 i = 0; i < Indices.Num(); i += 3)
    {
        T& V0 = Vertices[static_cast<int32>(Indices[i])];
        T& V1 = Vertices[static_cast<int32>(Indices[i + 1])];
        T& V2 = Vertices[static_cast<int32>(Indices[i + 2])];
        
        CalculateTangent_Internal(V0, V1, V2);
        CalculateTangent_Internal(V1, V2, V0);
        CalculateTangent_Internal(V2, V0, V1);
    }

    // 각 정점의 탄젠트 정규화
    for (T& Vertex : Vertices)
    {
        FVector Tangent(Vertex.TangentX, Vertex.TangentY, Vertex.TangentZ);
        if (!Tangent.IsNearlyZero())
        {
            Tangent.Normalize();
        }
        else
        {
            // 탄젠트를 계산할 수 없는 경우 기본값 설정
            FVector Normal(Vertex.NormalX, Vertex.NormalY, Vertex.NormalZ);
            FVector Arbitrary = FMath::Abs(Normal.Z) < 0.99f ? FVector(0, 0, 1) : FVector(1, 0, 0);
            Tangent = FVector::CrossProduct(Normal, Arbitrary).GetSafeNormal();
        }
        
        Vertex.TangentX = Tangent.X;
        Vertex.TangentY = Tangent.Y;
        Vertex.TangentZ = Tangent.Z;
    }
}

// FbxLayerElementTemplate에서 데이터를 가져오는 일반화된 헬퍼 함수
template<typename FbxLayerElementType, typename TDataType>
bool GetVertexElementData(const FbxLayerElementType* Element, int32 ControlPointIndex, int32 VertexIndex, TDataType& OutData)
{
    if (!Element)
    {
        return false;
    }

    const auto MappingMode = Element->GetMappingMode();
    const auto ReferenceMode = Element->GetReferenceMode();

    // eAllSame: 모든 정점이 같은 값
    if (MappingMode == FbxLayerElement::eAllSame)
    {
        if (Element->GetDirectArray().GetCount() > 0)
        {
            OutData = Element->GetDirectArray().GetAt(0);
            return true;
        }
        return false;
    }

    // 2) 인덱스 결정 (eByControlPoint, eByPolygonVertex만 처리)
    int32 Index = -1;
    if (MappingMode == FbxLayerElement::eByControlPoint)
    {
        Index = ControlPointIndex;
    }
    else if (MappingMode == FbxLayerElement::eByPolygonVertex)
    {
        Index = VertexIndex;
    }
    else
    {
        // eByPolygon, eByEdge 등 필요시 추가
        return false;
    }

    // 3) ReferenceMode별 분리 처리
    if (ReferenceMode == FbxLayerElement::eDirect)
    {
        // DirectArray 크기만 검사
        if (Index >= 0 && Index < Element->GetDirectArray().GetCount())
        {
            OutData = Element->GetDirectArray().GetAt(Index);
            return true;
        }
    }
    else if (ReferenceMode == FbxLayerElement::eIndexToDirect)
    {
        // IndexArray, DirectArray 순차 검사
        if (Index >= 0 && Index < Element->GetIndexArray().GetCount())
        {
            int32 DirectIndex = Element->GetIndexArray().GetAt(Index);
            if (DirectIndex >= 0 && DirectIndex < Element->GetDirectArray().GetCount())
            {
                OutData = Element->GetDirectArray().GetAt(DirectIndex);
                return true;
            }
        }
    }

    return false;
}

/**
 * Fills
 * 1. AbsoluteFilePath
 * 2. AssetName, which is used in texture asset naming
 */
FTextureLoadData GetImageInfo(FbxFileTexture* FbxTexture)
{
    FTextureLoadData LoadResult;

    LoadResult.AbsoluteFilePath = FbxTexture->GetFileName();
    LoadResult.AssetName = FbxTexture->GetName();

    return LoadResult;
}

/**
 * Extracts material properties and associated textures from an FBX material
 */
void GetMaterialInfo(FbxSurfaceMaterial* FbxMaterial, FMaterialLoadData& OutMaterial, TArray<FTextureLoadData>& OutTextures)
{
    OutMaterial.AssetName = FbxMaterial->GetName();

    /**
     * Material values
     */
    if (FbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
    {
        FbxSurfaceLambert* Lambert = static_cast<FbxSurfaceLambert*>(FbxMaterial);
        
        FbxDouble3 Diffuse = Lambert->Diffuse.Get();
        OutMaterial.DiffuseColor = FVector(
            static_cast<float>(Diffuse[0]), 
            static_cast<float>(Diffuse[1]), 
            static_cast<float>(Diffuse[2])
        );
        
        FbxDouble3 Ambient = Lambert->Ambient.Get();
        OutMaterial.AmbientColor = FVector(
            static_cast<float>(Ambient[0]), 
            static_cast<float>(Ambient[1]), 
            static_cast<float>(Ambient[2])
        );
        
        FbxDouble3 Emissive = Lambert->Emissive.Get();
        OutMaterial.EmissiveColor = FVector(
            static_cast<float>(Emissive[0]), 
            static_cast<float>(Emissive[1]), 
            static_cast<float>(Emissive[2])
        );
        
        // 투명도 처리
        float Transparency = static_cast<float>(1.0 - Lambert->TransparencyFactor.Get());
        OutMaterial.Transparency = Transparency;
        OutMaterial.bTransparent = (Transparency < 1.0f);
    }
    
    // Phong 머티리얼 추가 속성 (Lambert를 상속함)
    if (FbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
    {
        FbxSurfacePhong* Phong = static_cast<FbxSurfacePhong*>(FbxMaterial);
        
        FbxDouble3 Specular = Phong->Specular.Get();
        OutMaterial.SpecularColor = FVector(
            static_cast<float>(Specular[0]), 
            static_cast<float>(Specular[1]), 
            static_cast<float>(Specular[2])
        );
        
        OutMaterial.Shininess = static_cast<float>(Phong->Shininess.Get());
        
        OutMaterial.IOR = static_cast<float>(Phong->ReflectionFactor.Get());

        // Phong to MetallicRoughness
        // from Unreal Engine MF_PhongToMetalRoughness
        OutMaterial.Metallic = (OutMaterial.AmbientColor / 3.f).X;
        OutMaterial.Roughness = FMath::Clamp(FMath::Pow(2.f / (FMath::Clamp(OutMaterial.Shininess, 2.f, 1000.f) + 2.f), 0.25f), 0.f, 1.f);
    }

    const char* FbxTextureTypes[] = {
        FbxSurfaceMaterial::sDiffuse,
        FbxSurfaceMaterial::sSpecular,
        FbxSurfaceMaterial::sNormalMap,
        FbxSurfaceMaterial::sEmissive,
        FbxSurfaceMaterial::sTransparencyFactor,
        FbxSurfaceMaterial::sAmbient,
        FbxSurfaceMaterial::sShininess,
        FbxSurfaceMaterial::sReflectionFactor,
    };

    /**
     * Textures
     */
    auto ProcessTextureType = [&](const char* TextureType, ETextureType MaterialTextureType)
    {
        FbxProperty Property = FbxMaterial->FindProperty(TextureType);
        if (Property.IsValid())
        {
            const int32 TextureCount = FbxMaterial->GetSrcObjectCount<FbxFileTexture>();
            for (int32 TexIdx = 0; TexIdx < TextureCount; ++TexIdx)
            {
                FbxFileTexture* FbxTexture = FbxMaterial->GetSrcObject<FbxFileTexture>(TexIdx);
                if (FbxTexture)
                {
                    FTextureLoadData ImageResult = GetImageInfo(FbxTexture);
                    OutMaterial.TexturePaths.Add({MaterialTextureType, ImageResult.AbsoluteFilePath});
                    OutTextures.Add(ImageResult);
                }
            }
        }
    };

    ProcessTextureType(FbxSurfaceMaterial::sDiffuse, ETextureType::Diffuse);
    ProcessTextureType(FbxSurfaceMaterial::sSpecular, ETextureType::Specular);
    ProcessTextureType(FbxSurfaceMaterial::sNormalMap, ETextureType::Normal);
    ProcessTextureType(FbxSurfaceMaterial::sEmissive, ETextureType::Emissive);
    ProcessTextureType(FbxSurfaceMaterial::sTransparencyFactor, ETextureType::Alpha);
    ProcessTextureType(FbxSurfaceMaterial::sAmbient, ETextureType::Ambient);
    ProcessTextureType(FbxSurfaceMaterial::sShininess, ETextureType::Shininess);
    ProcessTextureType(FbxSurfaceMaterial::sReflectionFactor, ETextureType::Metallic);
}


/**
 * Determines if the given node is a root skeleton node
 * A root skeleton node is defined as a skeleton node that has no parent skeleton node
 */
bool IsSkeletonRootNode(FbxNode* Node)
{
    if (!Node)
    {
        return false;
    }
    
    FbxNodeAttribute* Attribute = Node->GetNodeAttribute();
    if (Attribute && Attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        // 부모가 없거나 부모가 스켈레톤이 아닌 경우에만 루트로 간주
        FbxNode* Parent = Node->GetParent();
        if (Parent == nullptr || Parent->GetNodeAttribute() == nullptr || 
            Parent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton)
        {
            return true;
        }
    }
    return false;
}

/**
 * Recursively collects all root skeleton nodes
 * Starting from the root skeleton node, we build individual skeletons
 */
void FindSkeletonRootNodes(FbxNode* Node, TArray<FbxNode*>& OutSkeletonRoots)
{
    if (!Node)
    {
        return;
    }

    if (IsSkeletonRootNode(Node))
    {
        OutSkeletonRoots.Add(Node);
    }

    const int32 ChildCount = Node->GetChildCount();
    for (int32 ChildIdx = 0; ChildIdx < ChildCount; ++ChildIdx)
    {
        FindSkeletonRootNodes(Node->GetChild(ChildIdx), OutSkeletonRoots);
    }
}

/**
 * Recursively collects all skeleton bone nodes under the given node
 */
void CollectSkeletonBoneNodes(FbxNode* Node, TArray<FbxNode*>& OutBoneNodes)
{
    if (!Node)
    {
        return;
    }

    FbxNodeAttribute* Attribute = Node->GetNodeAttribute();
    if (Attribute && Attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        OutBoneNodes.Add(Node);
    }

    const int32 ChildCount = Node->GetChildCount();
    for (int32 ChildIdx = 0; ChildIdx < ChildCount; ++ChildIdx)
    {
        CollectSkeletonBoneNodes(Node->GetChild(ChildIdx), OutBoneNodes);
    }
}

/**
 * Returns the index of the skeleton associated with the given mesh node
 */
int32 FindAssociatedSkeleton(FbxNode* MeshNode, const TArray<FReferenceSkeleton>& Skeletons)
{
    if (!MeshNode || Skeletons.Num() == 0)
    {
        return INDEX_NONE;
    }
    
    FbxMesh* Mesh = MeshNode->GetMesh();
    if (!Mesh)
    {
        return INDEX_NONE;
    }
    
    // 스킨 데이터가 있는지 확인
    bool bHasSkin = false;
    TSet<FbxNode*> BoneNodes;
    
    // 모든 스킨 디포머 순회
    for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(FbxDeformer::eSkin); ++DeformerIdx)
    {
        FbxSkin* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(DeformerIdx, FbxDeformer::eSkin));
        if (!Skin)
        {
            continue;
        }
        
        bHasSkin = true;
        
        // 모든 클러스터 순회하여 본 노드 수집
        for (int32 ClusterIdx = 0; ClusterIdx < Skin->GetClusterCount(); ++ClusterIdx)
        {
            FbxCluster* Cluster = Skin->GetCluster(ClusterIdx);
            if (Cluster && Cluster->GetLink())
            {
                BoneNodes.Add(Cluster->GetLink());
            }
        }
    }
    
    if (!bHasSkin || BoneNodes.Num() == 0)
    {
        return INDEX_NONE; // 스킨 데이터가 없으면 스태틱 메시로 간주
    }
    
    // 가장 많은 본을 공유하는 스켈레톤 찾기
    int32 BestMatchIndex = INDEX_NONE;
    int32 MaxSharedBones = 0;

    for (int32 SkeletonIdx = 0; SkeletonIdx < Skeletons.Num(); ++SkeletonIdx)
    {
        const FReferenceSkeleton& RefSkeleton = Skeletons[SkeletonIdx];
        int32 SharedBones = 0;
        
        for (FbxNode* BoneNode : BoneNodes)
        {
            FName BoneName(BoneNode->GetName());
            if (RefSkeleton.FindBoneIndex(BoneName) != INDEX_NONE)
            {
                SharedBones++;
            }
        }
        
        if (SharedBones > MaxSharedBones)
        {
            MaxSharedBones = SharedBones;
            BestMatchIndex = SkeletonIdx;
        }
    }

    return BestMatchIndex;
}

/**
 * Recursively collects mesh nodes and categorizes them into skeletal and static meshes
 * key for OutSkeletalMeshNodes is the index of the associated skeleton in the Skeletons array
 */
void CollectMeshNodes(FbxNode* Node, 
    const TArray<FReferenceSkeleton>& Skeletons, 
    TMap<int32, TArray<FbxNode*>>& OutSkeletalMeshNodes,
    TArray<FbxNode*>& OutStaticMeshNodes)
{
    if (Node && Node->GetNodeAttribute() && 
        Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* Mesh = Node->GetMesh();
        if (!Mesh)
        {
            return;
        }
        
        // 먼저 스킨 데이터가 있는지 확인하여 메시 유형 결정
        bool bHasSkin = false;
        for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(); ++DeformerIdx)
        {
            FbxDeformer* Deformer = Mesh->GetDeformer(DeformerIdx);
            if (Deformer && Deformer->GetDeformerType() == FbxDeformer::eSkin)
            {
                bHasSkin = true;
                break;
            }
        }

        int32 AssociatedSkeletonIndex = INDEX_NONE;
        if (bHasSkin)
        {
            // 이 메시와 연결된 스켈레톤 찾기
            AssociatedSkeletonIndex = FindAssociatedSkeleton(Node, Skeletons);
        }
        
        if (AssociatedSkeletonIndex != INDEX_NONE)
        {
            // 스켈레탈 메시
            OutSkeletalMeshNodes.FindOrAdd(AssociatedSkeletonIndex).Add(Node);
        }
        else
        {
            // 스태틱 메시
            OutStaticMeshNodes.Add(Node);
        }
    }
    
    // 자식 노드 재귀 처리
    for (int i = 0; i < Node->GetChildCount(); i++)
    {
        CollectMeshNodes(Node->GetChild(i), Skeletons, OutSkeletalMeshNodes, OutStaticMeshNodes);
    }
}

bool CreateStaticMesh(FbxNode* MeshNode, int32 MeshIndex, FStaticMeshLoadData& OutStaticMesh)
{
    if (!MeshNode)
    {
        return false;
    }
    
    FbxMesh* Mesh = MeshNode->GetMesh();
    if (!Mesh)
    {
        return false;
    }
    
    FString AssetName = MeshNode->GetDocument()->GetName();
    OutStaticMesh.AssetName = MeshIndex == 0 ? AssetName : AssetName + FString::FromInt(MeshIndex);
    
    uint32 RunningIndex = 0;

    // 레이어 요소 가져오기 (UV, Normal, Tangent, Color 등은 레이어에 저장됨)
    // 보통 Layer 0을 사용
    FbxLayer* BaseLayer = Mesh->GetLayer(0);
    if (!BaseLayer)
    {
        UE_LOG(ELogLevel::Error, TEXT("Mesh has no Layer 0."));
        return false;
    }
    
    const FbxAMatrix LocalTransformMatrix = MeshNode->EvaluateLocalTransform();

    // 정점 데이터 추출 및 병합
    const int32 PolygonCount = Mesh->GetPolygonCount(); // 삼각형 개수 (Triangulate 후)
    const FbxVector4* ControlPoints = Mesh->GetControlPoints(); // 제어점 (정점 위치) 배열
    const int32 ControlPointsCount = Mesh->GetControlPointsCount();

    // 정점 병합을 위한 맵
    TMap<FVertexKey, uint32> UniqueVertices;

    const FbxLayerElementNormal* NormalElement = BaseLayer->GetNormals();
    const FbxLayerElementTangent* TangentElement = BaseLayer->GetTangents();
    const FbxLayerElementUV* UVElement = BaseLayer->GetUVs();
    const FbxLayerElementVertexColor* ColorElement = BaseLayer->GetVertexColors();

    TMap<int32, TArray<IndexType>> TempMaterialIndices; //MaterialIndex별 인덱스 배열

    int VertexCounter = 0; // 폴리곤 정점 인덱스 (eByPolygonVertex 모드용)

    // 폴리곤(삼각형) 순회
    for (int32 i = 0; i < PolygonCount; ++i)
    {
        int32 MaterialIndex = 0;
        FbxGeometryElementMaterial* MaterialElement = Mesh->GetElementMaterial();
        if (MaterialElement)
        {
            auto Mode = MaterialElement->GetMappingMode();
            if (Mode == FbxGeometryElement::eByPolygon)
            {
                MaterialIndex = MaterialElement->GetIndexArray().GetAt(i);
            }
            else if (Mode == FbxGeometryElement::eAllSame)
            {
                MaterialIndex = MaterialElement->GetIndexArray().GetAt(0);
            }
        }

        IndexType PolyIndices[3];
        // 각 폴리곤(삼각형)의 정점 3개 순회
        for (int32 j = 0; j < 3; ++j)
        {
            const int32 ControlPointIndex = Mesh->GetPolygonVertex(i, j);

            FbxVector4 Position = ControlPoints[ControlPointIndex];
            FbxVector4 Normal;
            FbxVector4 Tangent;
            FbxVector2 UV;
            FbxColor Color;
            
            int NormalIndex = (NormalElement) ? (NormalElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
            int TangentIndex = (TangentElement) ? (TangentElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
            int UVIndex = (UVElement) ? (UVElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex ? Mesh->GetTextureUVIndex(i, j) : ControlPointIndex) : -1;
            int ColorIndex = (ColorElement) ? (ColorElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
            
            IndexType NewIndex;

            // 정점 병합 키 생성
            FVertexKey Key(ControlPointIndex, NormalIndex, TangentIndex, UVIndex, ColorIndex);

            // 맵에서 키 검색
            if (const uint32* Found = UniqueVertices.Find(Key))
            {
                NewIndex = *Found;
            }
            else
            {
                FStaticMeshVertex NewVertex;

                // Position
                if (ControlPointIndex < ControlPointsCount)
                {
                    Position = LocalTransformMatrix.MultT(Position);
                    SetVertexPosition(NewVertex, Position);
                }

                // Normal
                if (NormalElement && GetVertexElementData(NormalElement, ControlPointIndex, VertexCounter, Normal))
                {
                    Normal = LocalTransformMatrix.Inverse().Transpose().MultT(Normal);
                    SetVertexNormal(NewVertex, Normal);
                }

                // Tangent
                if (TangentElement && GetVertexElementData(TangentElement, ControlPointIndex, VertexCounter, Tangent))
                {
                     SetVertexTangent(NewVertex, Tangent);
                }

                // UV
                if(UVElement && GetVertexElementData(UVElement, ControlPointIndex, VertexCounter, UV))
                {
                    SetVertexUV(NewVertex, UV);
                }

                // Vertex Color
                if (ColorElement && GetVertexElementData(ColorElement, ControlPointIndex, VertexCounter, Color))
                {
                     SetVertexColor(NewVertex, Color);
                }
                
                // 새로운 정점을 Vertices 배열에 추가
                OutStaticMesh.VerticesPositionOnly.Add({NewVertex.X, NewVertex.Y, NewVertex.Z});
                OutStaticMesh.Vertices.Emplace(NewVertex);
                // 새 정점의 인덱스 계산
                NewIndex = static_cast<uint32>(OutStaticMesh.Vertices.Num() - 1);
                // 맵에 새 정점 정보 추가
                UniqueVertices.Add(Key, NewIndex);
            }
            PolyIndices[j] = NewIndex;
            VertexCounter++; // 다음 폴리곤 정점으로 이동
        } // End for each vertex in polygon

        // 머티리얼별 인덱스 배열에 이 삼각형의 인덱스 3개 추가
        TempMaterialIndices.FindOrAdd(MaterialIndex).Add(PolyIndices[0]);
        TempMaterialIndices.FindOrAdd(MaterialIndex).Add(PolyIndices[1]);
        TempMaterialIndices.FindOrAdd(MaterialIndex).Add(PolyIndices[2]);
    } // End for each polygon

    FbxNode* OwnerNode = Mesh->GetNode();

    for (auto& Pair : TempMaterialIndices)
    {
        int32 MatIdx = Pair.Key;
        const TArray<IndexType>& Indices = Pair.Value;

        FStaticSubMeshInfo SubMesh;
        SubMesh.MaterialIndex = MatIdx;
        SubMesh.IndexStart = RunningIndex;
        SubMesh.IndexCount = Indices.Num();
        
        OutStaticMesh.SubMeshes.Emplace(SubMesh);
        OutStaticMesh.Indices.Append(Indices);
        RunningIndex += Indices.Num();
    }

    CalculateTangents(OutStaticMesh.Vertices, OutStaticMesh.Indices);
    
    return true;
}

/**
 * @param MeshNodes Nodes that compose the new skeletal mesh
 * @param ReferenceSkeleton The reference skeleton to bind the mesh to
 * @param MeshIndex One FBX file can have multiple meshes. Used for naming this mesh.
 * @param OutSkeletalMesh The output skeletal mesh load data
 * @return True if the skeletal mesh was created successfully, false otherwise
 */
bool CreateSkeletalMeshFromNodes(const TArray<FbxNode*>& MeshNodes, 
    const FReferenceSkeleton& ReferenceSkeleton, int32 MeshIndex, FSkeletalMeshLoadData& OutSkeletalMesh)
{
    if (MeshNodes.IsEmpty())
    {
        UE_LOG(ELogLevel::Warning, TEXT("No mesh nodes provided for skeletal mesh creation."));
        return false;
    }

    FString AssetName = MeshNodes[0]->GetDocument()->GetName();
    OutSkeletalMesh.AssetName = MeshIndex == 0 ? AssetName : AssetName + FString::FromInt(MeshIndex);
    
    uint32 RunningIndex = 0;

    for (FbxNode* Node : MeshNodes)
    {
        FbxMesh* Mesh = Node->GetMesh();
        if (!Mesh)
        {
            continue;
        }
        
        // 레이어 요소 가져오기 (UV, Normal, Tangent, Color 등은 레이어에 저장됨)
        // 보통 Layer 0을 사용
        FbxLayer* BaseLayer = Mesh->GetLayer(0);
        if (!BaseLayer)
        {
            UE_LOG(ELogLevel::Error, TEXT("Error: Mesh has no Layer 0."));
            return false;
        }
        
        const FbxAMatrix LocalTransformMatrix = Node->EvaluateLocalTransform();

        // 정점 데이터 추출 및 병합
        const int32 PolygonCount = Mesh->GetPolygonCount(); // 삼각형 개수 (Triangulate 후)
        const FbxVector4* ControlPoints = Mesh->GetControlPoints(); // 제어점 (정점 위치) 배열
        const int32 ControlPointsCount = Mesh->GetControlPointsCount();

        // 정점 병합을 위한 맵
        TMap<FVertexKey, uint32> UniqueVertices;

        const FbxLayerElementNormal* NormalElement = BaseLayer->GetNormals();
        const FbxLayerElementTangent* TangentElement = BaseLayer->GetTangents();
        const FbxLayerElementUV* UVElement = BaseLayer->GetUVs();
        const FbxLayerElementVertexColor* ColorElement = BaseLayer->GetVertexColors();

        // 컨트롤 포인트별 본·스킨 가중치 맵
        TMap<int32, TArray<TPair<int32, double>>> SkinWeightMap;
        for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(FbxDeformer::eSkin); ++DeformerIdx)
        {
            FbxSkin* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(DeformerIdx, FbxDeformer::eSkin));
            for (int32 ClusterIdx = 0; ClusterIdx < Skin->GetClusterCount(); ++ClusterIdx)
            {
                FbxCluster* Cluster = Skin->GetCluster(ClusterIdx);
                FbxNode* LinkNode = Cluster->GetLink();
                if (!LinkNode)
                {
                    continue;
                }
                
                int32 BoneIndex = -1;
                // if (ReferenceSkeleton)
                // {
                    BoneIndex = ReferenceSkeleton.FindBoneIndex(LinkNode->GetName());
                // }
                if (BoneIndex < 0)
                {
                    continue;
                }
                
                int32 ControlPointCount = Cluster->GetControlPointIndicesCount();
                int32* ControlPointIndices = Cluster->GetControlPointIndices();
                double* ControlPointWeights = Cluster->GetControlPointWeights();
            
                for (int ControlPointIdx = 0; ControlPointIdx < ControlPointCount; ++ControlPointIdx)
                {
                    int32 ControlPoint = ControlPointIndices[ControlPointIdx];
                    double Weight = ControlPointWeights[ControlPointIdx];
                
                    if (Weight > 0.0)
                    {
                        SkinWeightMap.FindOrAdd(ControlPoint).Add(TPair(BoneIndex, Weight));
                    }
                }
            }
        }

        TMap<int32, TArray<IndexType>> TempMaterialIndices; //MaterialIndex별 인덱스 배열

        int VertexCounter = 0; // 폴리곤 정점 인덱스 (eByPolygonVertex 모드용)

        // 폴리곤(삼각형) 순회
        for (int32 i = 0; i < PolygonCount; ++i)
        {
            int32 MaterialIndex = 0;
            FbxGeometryElementMaterial* MaterialElement = Mesh->GetElementMaterial();
            if (MaterialElement)
            {
                auto Mode = MaterialElement->GetMappingMode();
                if (Mode == FbxGeometryElement::eByPolygon)
                {
                    MaterialIndex = MaterialElement->GetIndexArray().GetAt(i);
                }
                else if (Mode == FbxGeometryElement::eAllSame)
                {
                    MaterialIndex = MaterialElement->GetIndexArray().GetAt(0);
                }
            }

            IndexType PolyIndices[3];
            // 각 폴리곤(삼각형)의 정점 3개 순회
            for (int32 j = 0; j < 3; ++j)
            {
                const int32 ControlPointIndex = Mesh->GetPolygonVertex(i, j);

                FbxVector4 Position = ControlPoints[ControlPointIndex];
                FbxVector4 Normal;
                FbxVector4 Tangent;
                FbxVector2 UV;
                FbxColor Color;
                
                int NormalIndex = (NormalElement) ? (NormalElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
                int TangentIndex = (TangentElement) ? (TangentElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
                int UVIndex = (UVElement) ? (UVElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex ? Mesh->GetTextureUVIndex(i, j) : ControlPointIndex) : -1;
                int ColorIndex = (ColorElement) ? (ColorElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
                
                uint32 NewIndex;

                // 정점 병합 키 생성
                FVertexKey Key(ControlPointIndex, NormalIndex, TangentIndex, UVIndex, ColorIndex);

                // 맵에서 키 검색
                if (const uint32* Found = UniqueVertices.Find(Key))
                {
                    NewIndex = *Found;
                }
                else
                {
                    FSkeletalMeshVertex NewVertex;

                    // Position
                    if (ControlPointIndex < ControlPointsCount)
                    {
                        Position = LocalTransformMatrix.MultT(Position);
                        SetVertexPosition(NewVertex, Position);
                    }

                    // Normal
                    if (NormalElement && GetVertexElementData(NormalElement, ControlPointIndex, VertexCounter, Normal))
                    {
                        Normal = LocalTransformMatrix.Inverse().Transpose().MultT(Normal);
                        SetVertexNormal(NewVertex, Normal);
                    }

                    // Tangent
                    if (TangentElement && GetVertexElementData(TangentElement, ControlPointIndex, VertexCounter, Tangent))
                    {
                         SetVertexTangent(NewVertex, Tangent);
                    }

                    // UV
                    if(UVElement && GetVertexElementData(UVElement, ControlPointIndex, VertexCounter, UV))
                    {
                        SetVertexUV(NewVertex, UV);
                    }

                    // Vertex Color
                    if (ColorElement && GetVertexElementData(ColorElement, ControlPointIndex, VertexCounter, Color))
                    {
                         SetVertexColor(NewVertex, Color);
                    }

                    // 본 데이터 설정
                    auto& InfluenceList = SkinWeightMap[ControlPointIndex];
                    std::sort(InfluenceList.begin(), InfluenceList.end(),
                        [](auto const& A, auto const& B)
                        {
                            return A.Value > B.Value; // Weight 기준 내림차순 정렬
                        }
                    );
                    
                    double TotalWeight = 0.0;
                    for (int32 BoneIdx = 0; BoneIdx < 4 && BoneIdx < InfluenceList.Num(); ++BoneIdx)
                    {
                        NewVertex.BoneIndices[BoneIdx] = InfluenceList[BoneIdx].Key;
                        NewVertex.BoneWeights[BoneIdx] = static_cast<float>(InfluenceList[BoneIdx].Value);
                        TotalWeight += InfluenceList[BoneIdx].Value;
                    }
                    if (TotalWeight > 0.0)
                    {
                        for (int BoneIdx = 0; BoneIdx < 4; ++BoneIdx)
                        {
                            NewVertex.BoneWeights[BoneIdx] /= static_cast<float>(TotalWeight);
                        }
                    }

                    // 새로운 정점을 Vertices 배열에 추가
                    OutSkeletalMesh.VerticesPositionOnly.Add({NewVertex.X, NewVertex.Y, NewVertex.Z});
                    OutSkeletalMesh.Vertices.Emplace(NewVertex);
                    // 새 정점의 인덱스 계산
                    NewIndex = static_cast<uint32>(OutSkeletalMesh.Vertices.Num() - 1);
                    // 맵에 새 정점 정보 추가
                    UniqueVertices.Add(Key, NewIndex);
                }
                PolyIndices[j] = NewIndex;
                VertexCounter++; // 다음 폴리곤 정점으로 이동
            } // End for each vertex in polygon

            // 머티리얼별 인덱스 배열에 이 삼각형의 인덱스 3개 추가
            TempMaterialIndices.FindOrAdd(MaterialIndex).Add(PolyIndices[0]);
            TempMaterialIndices.FindOrAdd(MaterialIndex).Add(PolyIndices[1]);
            TempMaterialIndices.FindOrAdd(MaterialIndex).Add(PolyIndices[2]);
        } // End for each polygon

        FbxNode* OwnerNode = Mesh->GetNode();

        for (auto& Pair : TempMaterialIndices)
        {
            int32 MatIdx = Pair.Key;
            const TArray<IndexType>& Indices = Pair.Value;

            FSkeletalSubMeshInfo SubMesh;
            SubMesh.MaterialIndex = MatIdx;
            SubMesh.IndexStart = RunningIndex;
            SubMesh.IndexCount = Indices.Num();

            OutSkeletalMesh.SubMeshes.Emplace(SubMesh);
            OutSkeletalMesh.Indices += Indices;
            RunningIndex += Indices.Num();
        }
    }

    CalculateTangents(OutSkeletalMesh.Vertices, OutSkeletalMesh.Indices);
    
    return true;
}

FbxPose* FindBindPose(FbxScene* Scene, FbxNode* SkeletonRoot)
{
    if (!Scene || !SkeletonRoot)
    {
        return nullptr;
    }

    // 스켈레톤에 속한 모든 본 노드를 수집
    TArray<FbxNode*> SkeletonBones;
    CollectSkeletonBoneNodes(SkeletonRoot, SkeletonBones);
    
    const int32 PoseCount = Scene->GetPoseCount();
    for (int32 PoseIndex = 0; PoseIndex < PoseCount; PoseIndex++)
    {
        FbxPose* CurrentPose = Scene->GetPose(PoseIndex);
        if (!CurrentPose || !CurrentPose->IsBindPose())
        {
            continue;
        }
            
        // 이 바인드 포즈가 스켈레톤의 일부 본을 포함하는지 확인
        bool bPoseContainsSomeBones = false;
        int32 NodeCount = CurrentPose->GetCount();
        
        for (int32 NodeIndex = 0; NodeIndex < NodeCount; NodeIndex++)
        {
            FbxNode* Node = CurrentPose->GetNode(NodeIndex);
            if (SkeletonBones.Contains(Node))
            {
                bPoseContainsSomeBones = true;
                break;
            }
        }
        
        // 이 스켈레톤에 바인드 포즈가 적어도 하나의 본을 포함하면 반환
        if (bPoseContainsSomeBones)
        {
            return CurrentPose;
        }
    }
    
    return nullptr; // 해당 스켈레톤에 관련된 바인드 포즈 없음
}

void CollectBoneData(FbxNode* Node, FReferenceSkeleton& OutReferenceSkeleton, int32 ParentIndex, FbxPose* BindPose)
{
    if (!Node)
    {
        return;
    }
    
    TArray<FMeshBoneInfo>& RefBoneInfo = OutReferenceSkeleton.RawRefBoneInfo;
    TArray<FTransform>& RefBonePose = OutReferenceSkeleton.RawRefBonePose;
    TMap<FName, int32>& NameToIndexMap = OutReferenceSkeleton.RawNameToIndexMap;
    TArray<FMatrix>& InverseBindPoseMatrices = OutReferenceSkeleton.InverseBindPoseMatrices;
    
    FName BoneName = FName(Node->GetName());
    const int32 CurrentIndex = RefBoneInfo.Num();
    NameToIndexMap.Add(BoneName, CurrentIndex);
    
    // 뼈 정보 추가
    FMeshBoneInfo BoneInfo(BoneName, ParentIndex);
    RefBoneInfo.Add(BoneInfo);

    // 레퍼런스 포즈
    FTransform BoneTransform;
    int32 PoseNodeIndex = INDEX_NONE;
    if (BindPose)
    {
        PoseNodeIndex = BindPose->Find(Node);
    }
    if (PoseNodeIndex != INDEX_NONE)
    {
        // 현재 노드의 글로벌 바인드 포즈 행렬 가져오기
        const FbxMatrix& NodeMatrix = BindPose->GetMatrix(PoseNodeIndex);
        FbxAMatrix NodeGlobalMatrix;
    
        // FbxMatrix를 FbxAMatrix로 변환
        for (int32 r = 0; r < 4; ++r)
        {
            for (int32 c = 0; c < 4; ++c)
            {
                NodeGlobalMatrix[r][c] = NodeMatrix.Get(r, c);
            }
        }
    
        // 로컬 트랜스폼 계산
        FbxAMatrix LocalMatrix;
    
        if (ParentIndex != INDEX_NONE)
        {
            // 부모 노드 찾기
            FbxNode* ParentNode = Node->GetParent();
            if (ParentNode)
            {
                // 부모 노드의 바인드 포즈 인덱스 찾기
                int32 ParentPoseIndex = BindPose->Find(ParentNode);
            
                if (ParentPoseIndex != INDEX_NONE)
                {
                    // 부모 노드의 글로벌 바인드 포즈 행렬 가져오기
                    FbxMatrix ParentNodeMatrix = BindPose->GetMatrix(ParentPoseIndex);
                    FbxAMatrix ParentGlobalMatrix;
                
                    // FbxMatrix를 FbxAMatrix로 변환
                    for (int r = 0; r < 4; ++r)
                    {
                        for (int c = 0; c < 4; ++c)
                        {
                            ParentGlobalMatrix[r][c] = ParentNodeMatrix.Get(r, c);
                        }
                    }
                    
                    // 로컬 트랜스폼 계산: Local = ParentGlobal^-1 * Global (FBX SDK는 열 우선)
                    LocalMatrix = ParentGlobalMatrix.Inverse() * NodeGlobalMatrix;
                }
                else
                {
                    // 부모의 바인드 포즈가 없으면 현재 노드의 로컬 트랜스폼 사용
                    LocalMatrix = Node->EvaluateLocalTransform();
                }
            }
            else
            {
                // 부모 노드가 없으면 현재 노드의 로컬 트랜스폼 사용
                LocalMatrix = Node->EvaluateLocalTransform();
            }
        }
        else
        {
            // 루트 노드는 글로벌 = 로컬
            LocalMatrix = NodeGlobalMatrix;
        }
    
        // FbxAMatrix를 FTransform으로 변환
        BoneTransform = FTransform(ConvertFbxMatrixToFMatrix(LocalMatrix));
    }
    else
    {
        // 현재 노드 변환 사용
        BoneTransform = ConvertFbxTransformToFTransform(Node);
    }
    BoneTransform.NormalizeRotation();
    RefBonePose.Add(BoneTransform);
    
    // 역 바인드 포즈
    FbxAMatrix GlobalBindPoseMatrix;
    if (PoseNodeIndex != INDEX_NONE)
    {
        GlobalBindPoseMatrix = ConvertFbxMatrixToFbxAMatrix(BindPose->GetMatrix(PoseNodeIndex));
    }
    else
    {
        GlobalBindPoseMatrix.SetIdentity();
    }
    FbxAMatrix InverseBindMatrix = GlobalBindPoseMatrix.Inverse();
    FMatrix InverseBindPoseMatrix = ConvertFbxMatrixToFMatrix(InverseBindMatrix);
    InverseBindPoseMatrices.Add(InverseBindPoseMatrix);
    
    // 자식 노드들을 재귀적으로 처리
    for (int i = 0; i < Node->GetChildCount(); i++)
    {
        FbxNode* ChildNode = Node->GetChild(i);
        if (ChildNode &&
            ChildNode->GetNodeAttribute() &&
            ChildNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            CollectBoneData(ChildNode, OutReferenceSkeleton, CurrentIndex, BindPose);
        }
    }
}

bool NodeHasAnimation(FbxNode* Node, FbxAnimLayer* AnimLayer)
{
    if (!Node || !AnimLayer)
    {
        return false;
    }
    
    // 위치, 회전, 크기 애니메이션 커브 확인
    FbxAnimCurve* TranslationX = Node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    FbxAnimCurve* TranslationY = Node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    FbxAnimCurve* TranslationZ = Node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    
    FbxAnimCurve* RotationX = Node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    FbxAnimCurve* RotationY = Node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    FbxAnimCurve* RotationZ = Node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    
    FbxAnimCurve* ScaleX = Node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    FbxAnimCurve* ScaleY = Node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    FbxAnimCurve* ScaleZ = Node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    
    // 하나라도 애니메이션 커브가 있고 키가 있으면 애니메이션이 있는 것으로 간주
    return (TranslationX && TranslationX->KeyGetCount() > 0) ||
           (TranslationY && TranslationY->KeyGetCount() > 0) ||
           (TranslationZ && TranslationZ->KeyGetCount() > 0) ||
           (RotationX && RotationX->KeyGetCount() > 0) ||
           (RotationY && RotationY->KeyGetCount() > 0) ||
           (RotationZ && RotationZ->KeyGetCount() > 0) ||
           (ScaleX && ScaleX->KeyGetCount() > 0) ||
           (ScaleY && ScaleY->KeyGetCount() > 0) ||
           (ScaleZ && ScaleZ->KeyGetCount() > 0);
}

/**
 * Collects names of nodes that have animation curves in the specified animation layer
 */
void CollectAnimationNodeNames(FbxNode* Node, FbxAnimLayer* AnimLayer, TSet<FString>& OutAnimationNodeNames)
{
    if (!Node)
    {
        return;
    }

    if (NodeHasAnimation(Node, AnimLayer))
    {
        OutAnimationNodeNames.Add(Node->GetName());
    }

    for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
    {
        CollectAnimationNodeNames(Node->GetChild(ChildIndex), AnimLayer, OutAnimationNodeNames);
    }
}

/**
 * Finds the best matching skeleton for the given animation stack and layer
 * by comparing animated bone names with skeleton bone names
 * @return Index of the best matching skeleton in the Skeletons array, or INDEX_NONE if no suitable match found
 * @note A single FBX file can contain multiple skeletons; this function helps identify which one matches the animation data
 */
int32 FindSkeletonForAnimation(FbxScene* Scene, FbxAnimStack* AnimStack, 
    FbxAnimLayer* AnimLayer, const TArray<FReferenceSkeleton>& Skeletons)
{
    if (!AnimStack || !AnimLayer || Skeletons.Num() == 0)
    {
        return INDEX_NONE;
    }
    
    // 애니메이션이 있는 본 노드 수집
    TSet<FString> AnimatedBoneNames;
    FbxNode* RootNode = Scene->GetRootNode();
    
    CollectAnimationNodeNames(RootNode, AnimLayer, AnimatedBoneNames);
    
    if (AnimatedBoneNames.Num() == 0)
    {
        return INDEX_NONE;
    }
    
    // 가장 많은 본을 공유하는 스켈레톤 찾기
    int32 BestMatchIndex = INDEX_NONE;
    int32 MaxSharedBones = 0;
    
    // for (const FReferenceSkeleton& Skeleton : Skeletons)
    for (int32 SkeletonIdx = 0; SkeletonIdx < Skeletons.Num(); ++SkeletonIdx)
    {
        int32 SharedBones = 0;
        const FReferenceSkeleton& Skeleton = Skeletons[SkeletonIdx];
        
        // 현재 스켈레톤의 모든 본 이름 확인
        for (int32 BoneIndex = 0; BoneIndex < Skeleton.GetRawBoneNum(); ++BoneIndex)
        {
            FName BoneName = Skeleton.GetBoneName(BoneIndex);
            if (AnimatedBoneNames.Contains(BoneName.ToString()))
            {
                SharedBones++;
            }
        }
        
        // 일치 비율 계산 (스켈레톤의 본 수에 대한 비율)
        float MatchRatio = static_cast<float>(SharedBones) / static_cast<float>(Skeleton.GetRawBoneNum());
        
        // 최소 50% 이상 일치하고, 지금까지 발견된 최대 일치 본 수보다 많으면 업데이트
        // if (MatchRatio >= 0.5f && SharedBones > MaxSharedBones)
        if (SharedBones > MaxSharedBones)
        {
            MaxSharedBones = SharedBones;
            BestMatchIndex = SkeletonIdx;
        }
    }
    
    return BestMatchIndex;
}

void BuildBoneNodeMap(FbxNode* Node, TMap<FName, FbxNode*>& OutBoneNodeMap)
{
    if (!Node)
    {
        return;
    }
    
    // 스켈레톤 노드만 맵에 추가 (필요에 따라 조건 수정)
    FbxNodeAttribute* Attribute = Node->GetNodeAttribute();
    if (Attribute && Attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        OutBoneNodeMap.Add(Node->GetName(), Node);
    }
    
    // 자식 노드들도 처리
    for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
    {
        BuildBoneNodeMap(Node->GetChild(ChildIndex), OutBoneNodeMap);
    }
}

void ExtractBoneAnimation(
    FbxScene* Scene, FbxNode* BoneNode, FbxTime Start, FbxTime End, int32 NumFrames,
    TArray<FVector>& OutPositions, TArray<FQuat>& OutRotations, TArray<FVector>& OutScales)
{
    // 배열 초기화
    OutPositions.Empty(NumFrames);
    OutRotations.Empty(NumFrames);
    OutScales.Empty(NumFrames);
    
    // 프레임 간격 계산
    FbxTime FrameTime;
    double FrameInterval = (End.GetSecondDouble() - Start.GetSecondDouble()) / (NumFrames - 1);
    FrameTime.SetSecondDouble(FrameInterval);
    
    // TODO: 바인드 포즈는 이전에 이미 찾았지만, 루트의 트랜스폼 관련 문제가 있어 다시 계산하는 방식 사용 중
    // 바인드 포즈 찾기
    FbxPose* BindPose = FindBindPose(Scene, BoneNode);
    int32 PoseNodeIndex = BindPose ? BindPose->Find(BoneNode) : -1;
    
    // 바인드 포즈 행렬 가져오기
    FbxAMatrix BindPoseMatrix;
    
    if (BindPose && PoseNodeIndex >= 0)
    {
        // 바인드 포즈에서 행렬 가져오기
        BindPoseMatrix = ConvertFbxMatrixToFbxAMatrix(BindPose->GetMatrix(PoseNodeIndex));
    }
    else
    {
        // 바인드 포즈가 없는 경우 노드의 기본 변환 사용
        BindPoseMatrix.SetT(BoneNode->LclTranslation.Get());
        BindPoseMatrix.SetR(BoneNode->LclRotation.Get());
        BindPoseMatrix.SetS(BoneNode->LclScaling.Get());
    }

    // 부모의 바인드 포즈 행렬 가져오기
    FbxPose* ParentBindPose = FindBindPose(Scene, BoneNode->GetParent());
    int32 ParentPoseNodeIndex = ParentBindPose ? ParentBindPose->Find(BoneNode->GetParent()) : -1;

    FbxAMatrix ParentBindPoseMatrix;

    if (ParentBindPose && ParentPoseNodeIndex >= 0)
    {
        ParentBindPoseMatrix = ConvertFbxMatrixToFbxAMatrix(ParentBindPose->GetMatrix(ParentPoseNodeIndex));
    }
    else
    {
        ParentBindPoseMatrix.SetT(BoneNode->GetParent()->LclTranslation.Get());
        ParentBindPoseMatrix.SetR(BoneNode->GetParent()->LclRotation.Get());
        ParentBindPoseMatrix.SetS(BoneNode->GetParent()->LclScaling.Get());
    }

    // 로컬 바인드 포즈
    FbxAMatrix LocalBindPoseMatrix = ParentBindPoseMatrix.Inverse() * BindPoseMatrix;

    // 바인드 포즈 행렬에서 위치, 회전, 스케일 추출
    FbxVector4 BindPoseTranslation = LocalBindPoseMatrix.GetT();
    FbxVector4 BindPoseScale = LocalBindPoseMatrix.GetS();
    FbxQuaternion BindPoseRotationQuat = LocalBindPoseMatrix.GetQ();
    FbxQuaternion BindInverseQuat = BindPoseRotationQuat;
    BindInverseQuat.Normalize();
    BindInverseQuat.Conjugate();
    
    // 각 프레임에 대해 변환 값 샘플링
    for (int32 FrameIndex = 0; FrameIndex < NumFrames; FrameIndex++)
    {
        // @todo : FrameTime이 현재 전체 시간인거 같은데, 확인필요
        FbxTime CurrentTime = Start + FrameTime * FrameIndex;
        
        // 애니메이션이 적용된 노드의 로컬 트랜스폼을 통해 오프셋 계산
        FbxAMatrix LocalTransform = BoneNode->EvaluateLocalTransform(CurrentTime);

        FbxAMatrix LocalOffsetMatrix = LocalBindPoseMatrix.Inverse() * LocalTransform;
        FbxQuaternion LocalRotationQuat = LocalOffsetMatrix.GetQ();
        FbxVector4 LocalTranslation = LocalOffsetMatrix.GetT();
        FbxVector4 LocalScale = LocalOffsetMatrix.GetS();

        FVector LocalPosition = FVector(
            static_cast<float>(LocalTranslation[0]),
            static_cast<float>(LocalTranslation[1]),
            static_cast<float>(LocalTranslation[2])
        );
        FQuat LocalRotation = FQuat(
            static_cast<float>(LocalRotationQuat[0]),
            static_cast<float>(LocalRotationQuat[1]),
            static_cast<float>(LocalRotationQuat[2]),
            static_cast<float>(LocalRotationQuat[3])
        );
        FVector LocalScale3D = FVector(
            static_cast<float>(LocalScale[0]),
            static_cast<float>(LocalScale[1]),
            static_cast<float>(LocalScale[2])
        );
        
        OutPositions.Add(LocalPosition);
        OutRotations.Add(LocalRotation);
        OutScales.Add(LocalScale3D);
    }
}