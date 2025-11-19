#include "MaterialLoader.h"

#include <fstream>
#include <sstream>
#include "Core/Container/StringConv.h"
#include "Engine/UserInterface/Console.h"

bool FMaterialLoader::LoadMaterial(const FFilePath& InFilePath, TArray<FMaterialLoadResult>& OutLoadResult)
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

        FMaterialLoadResult MaterialCurrentlyParsed;

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

            MaterialCurrentlyParsed = FMaterialLoadResult();

            MaterialCurrentlyParsed.bValid = true;
            MaterialCurrentlyParsed.AbsoluteFilePath = InFilePath;
            MaterialCurrentlyParsed.AssetName = Line;

            // constexpr uint32 TexturesNum = static_cast<uint32>(EMaterialTextureSlots::MTS_MAX);
            // Material.TextureInfos.SetNum(TexturesNum);

            // OutStaticMeshRenderData.Materials.Add(Material);
        }

        if (Token == "Kd")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.DiffuseColor = FVector(X, Y, Z);
        }
        if (Token == "Ks")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.SpecularColor = FVector(X, Y, Z);
        }
        if (Token == "Ka")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.AmbientColor = FVector(X, Y, Z);
        }
        if (Token == "Ke")
        {
            float X, Y, Z;
            LineStream >> X >> Y >> Z;
            MaterialCurrentlyParsed.EmissiveColor = FVector(X, Y, Z);
        }
        if (Token == "Ns")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Shininess = X;
        }
        if (Token == "Ni")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.IOR = X;
        }
        if (Token == "d" || Token == "Tr")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Transparency = (Token == "Tr") ? X : 1.f - X;
            MaterialCurrentlyParsed.bTransparent = true;
        }
        if (Token == "illum")
        {
            uint32 X;
            LineStream >> X;
            MaterialCurrentlyParsed.IlluminanceModel = X;
        }
        if (Token == "Pm")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Metallic = X;
        }
        if (Token == "Pr")
        {
            float X;
            LineStream >> X;
            MaterialCurrentlyParsed.Roughness = X;
        }

        /**
         * Texture maps
         * Mapping to FTexture will be resolved in UAssetManager.
         */
        if (Token == "map_Kd")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.DiffuseTexturePath = ResolvedTexturePath;

            // FWString TexturePath = OutObjInfo.FilePath + OutStaticMeshRenderData.Materials[MaterialIndex]->TextureInfos[SlotIdx].TextureName.ToWideString();
            // if (CreateTextureFromFile(TexturePath))
            // {
            //     OutStaticMeshRenderData.Materials[MaterialIndex]->TextureInfos[SlotIdx].TexturePath = TexturePath;
            //     OutStaticMeshRenderData.Materials[MaterialIndex]->TextureInfos[SlotIdx].bIsSRGB = true;
            //     OutStaticMeshRenderData.Materials[MaterialIndex]->TextureFlag |= static_cast<uint16>(EMaterialTextureFlags::MTF_Diffuse);
            // }
        }
        if (Token == "map_Bump")
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
                    MaterialCurrentlyParsed.NormalTexturePath = ResolvedTexturePath;
                }
            }
        }
        if (Token == "map_Ks")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.SpecularTexturePath = ResolvedTexturePath;
        }
        if (Token == "map_Ns")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.ShininessTexturePath = ResolvedTexturePath;
        }
        if (Token == "map_Ka")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.AmbientTexturePath = ResolvedTexturePath;
        }
        if (Token == "map_Ke")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.EmissiveTexturePath = ResolvedTexturePath;
        }
        if (Token == "map_Pm")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.MetallicTexturePath = ResolvedTexturePath;
        }
        if (Token == "map_Pr")
        {
            LineStream >> Line;
            FFilePath ResolvedTexturePath = ParentPath / Line;
            MaterialCurrentlyParsed.RoughnessTexturePath = ResolvedTexturePath;
        }
        // TODO: map_d 또는 map_Tr은 나중에 필요할때 구현
    }

    return true;

}