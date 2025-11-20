#include "MaterialLoader.h"

#include <fstream>
#include <sstream>
#include "Core/Container/StringConv.h"
#include "Engine/UserInterface/Console.h"

bool FMaterialLoader::LoadMaterial(const FFilePath& InFilePath, TArray<FMaterialLoadData>& OutLoadResult)
{
    // std::string FilePathStr = InFilePath.string();
    
    // std::string UTF8Path = WStringToString(InFilePath);
    std::ifstream MtlFile(InFilePath);
    if (!MtlFile.is_open())
    {
        UE_LOG(ELogLevel::Error, "FMaterialLoader::LoadMaterial - Failed to open MTL file: %s", InFilePath.c_str());
        return false;
    }

    FFilePath ParentPath = InFilePath.parent_path();

    std::string Line;
    // int32 MaterialIndex = -1;

    while (std::getline(MtlFile, Line))
    {
        if (Line.empty() || Line[0] == '#')
        {
            continue;
        }

        FMaterialLoadData MaterialCurrentlyParsed;

        std::istringstream LineStream(Line);
        std::string Token;
        LineStream >> Token;

        // Create new material if token is 'newmtl'
        if (Token == "newmtl")
        {
            LineStream >> Line;
            
            /**
             * Save previous material
             */
            if (MaterialCurrentlyParsed.bValid)
            {
                OutLoadResult.Add(std::move(MaterialCurrentlyParsed));
            }

            MaterialCurrentlyParsed = FMaterialLoadData();

            MaterialCurrentlyParsed.bValid = true;
            MaterialCurrentlyParsed.AbsoluteFilePath = InFilePath;
            MaterialCurrentlyParsed.AssetName = Line;

            // constexpr uint32 TexturesNum = static_cast<uint32>(EMaterialTextureSlots::MTS_MAX);
            // Material.TextureInfos.SetNum(TexturesNum);

            // OutStaticMeshRenderData.Materials.Add(Material);
        }
        else if (Token == "Kd")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.DiffuseColor = FVector(X, Y, Z);
        }
        else if (Token == "Ks")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.SpecularColor = FVector(X, Y, Z);
        }
        else if (Token == "Ka")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.AmbientColor = FVector(X, Y, Z);
        }
        else if (Token == "Ke")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.EmissiveColor = FVector(X, Y, Z);
        }
        else if (Token == "Ns")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Shininess = X;
        }
        else if (Token == "Ni")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.IOR = X;
        }
        else if (Token == "d" || Token == "Tr")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Transparency = (Token == "Tr") ? X : 1.f - X;
            MaterialCurrentlyParsed.bTransparent = true;
        }
        else if (Token == "illum")
        {
            uint32 X;
            LineStream >> X;
            MaterialCurrentlyParsed.IlluminanceModel = X;
        }
        else if (Token == "Pm")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Metallic = X;
        }
        else if (Token == "Pr")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Roughness = X;
        }

        /**
         * Texture maps
         * Mapping to FTexture will be resolved in UAssetManager.
         */
        else if (Token == "map_Kd")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Diffuse, ResolvedTexturePath);

            // FWString TexturePath = OutObjInfo.FilePath + OutStaticMeshRenderData.Materials[MaterialIndex]->TextureInfos[SlotIdx].TextureName.ToWideString();
            // if (CreateTextureFromFile(TexturePath))
            // {
            //     OutStaticMeshRenderData.Materials[MaterialIndex]->TextureInfos[SlotIdx].TexturePath = TexturePath;
            //     OutStaticMeshRenderData.Materials[MaterialIndex]->TextureInfos[SlotIdx].bIsSRGB = true;
            //     OutStaticMeshRenderData.Materials[MaterialIndex]->TextureFlag |= static_cast<uint16>(EMaterialTextureFlags::MTF_Diffuse);
            // }
        }
        else if (Token == "map_Bump")
        {   
            std::string Line;
            while (LineStream >> Line)
            {
                if (Line == "-bm")
                {
                    float BumpMultiplier;
                    LineStream >> BumpMultiplier;
                    MaterialCurrentlyParsed.BumpMultiplier = BumpMultiplier;
                }
                else
                {
                    FFilePath ResolvedTexturePath = ParentPath / Line;
                    MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Normal, ResolvedTexturePath);
                }
            }
        }
        else if (Token == "map_Ks")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Specular, ResolvedTexturePath);
        }
        else if (Token == "map_Ns")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Shininess, ResolvedTexturePath);
        }
        else if (Token == "map_Ka")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Ambient, ResolvedTexturePath);
        }
        else if (Token == "map_Ke")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Emissive, ResolvedTexturePath);
        }
        else if (Token == "map_Pm")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Metallic, ResolvedTexturePath);
        }
        else if (Token == "map_Pr")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Roughness, ResolvedTexturePath);
        }
        else if (Token.starts_with("map_"))
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.TexturePaths.Emplace(ETextureType::Unknown, ResolvedTexturePath);
        }

        // TODO: map_d 또는 map_Tr은 나중에 필요할때 구현
    }

    return true;

}