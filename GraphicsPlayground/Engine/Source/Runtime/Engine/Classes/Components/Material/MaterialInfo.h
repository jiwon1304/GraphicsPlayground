#pragma once

#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"
#include "Runtime/Launch/Define.h"
#include "Core/Serialization/Archive.h"

struct FMaterialInfo
{
    FString MaterialName;  // newmtl: Material Name.

    FWString MaterialPath;  // mtl file path

    uint32 TextureFlag = 0;

    bool bTransparent = false; // Has alpha channel?

    FVector DiffuseColor = FVector(0.7f, 0.7f, 0.7f);      // Kd: Diffuse Color
    FVector SpecularColor = FVector(0.5f, 0.5f, 0.5f);     // Ks: Specular Color
    FVector AmbientColor = FVector(0.01f, 0.01f , 0.01f);   // Ka: Ambient Color
    FVector EmissiveColor = FVector::ZeroVector;                   // Ke: Emissive Color

    float Shininess = 250.f;                                // Ns: Specular Power
    float IOR = 1.5f;                                              // Ni: Index of Refraction
    float Transparency = 0.f;                                      // d or Tr: Transparency of surface
    float BumpMultiplier = 1.f;                                    // -bm: Bump Multiplier
    uint32 IlluminanceModel;                                       // illum: illumination Model between 0 and 10.

    float Metallic = 0.0f;                                         // Pm: Metallic
    float Roughness = 0.5f;                                        // Pr: Roughness
    
    /* Texture */
    TArray<FTextureInfo> TextureInfos;

    void Serialize(FArchive& Ar)
    {
        Ar << MaterialName
           << TextureFlag
           << bTransparent
           << DiffuseColor
           << SpecularColor
           << AmbientColor
           << EmissiveColor
           << Shininess
           << IOR
           << Transparency
           << BumpMultiplier
           << IlluminanceModel
           << Metallic
           << Roughness
           << TextureInfos;
    }

    friend FArchive& operator<<(FArchive& Ar, FMaterialInfo& Info)
    {
        Info.Serialize(Ar);
        return Ar;
    }
};

inline bool operator==(const FMaterialInfo& A, const FMaterialInfo& B)
{
    return
        A.MaterialName == B.MaterialName &&
        A.MaterialPath == B.MaterialPath &&
        A.TextureFlag == B.TextureFlag &&
        A.bTransparent == B.bTransparent &&
        A.DiffuseColor == B.DiffuseColor &&
        A.SpecularColor == B.SpecularColor &&
        A.AmbientColor == B.AmbientColor &&
        A.EmissiveColor == B.EmissiveColor &&
        A.Shininess == B.Shininess &&
        A.IOR == B.IOR &&
        A.Transparency == B.Transparency &&
        A.BumpMultiplier == B.BumpMultiplier &&
        A.IlluminanceModel == B.IlluminanceModel &&
        A.Metallic == B.Metallic &&
        A.Roughness == B.Roughness &&
        A.TextureInfos == B.TextureInfos;
}

// 해시 결합 함수
inline void HashCombine(std::size_t& Seed, std::size_t Value)
{
    Seed ^= Value + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
}

struct FMaterialInfoHash
{
    std::size_t operator()(const FMaterialInfo& Info) const
    {
        std::size_t Seed = 0;
        HashCombine(Seed, std::hash<std::string>()(Info.MaterialName.ToUTF8String()));
        HashCombine(Seed, std::hash<uint32_t>()(Info.TextureFlag));
        HashCombine(Seed, std::hash<bool>()(Info.bTransparent));
        HashCombine(Seed, std::hash<float>()(Info.DiffuseColor.X));
        HashCombine(Seed, std::hash<float>()(Info.DiffuseColor.Y));
        HashCombine(Seed, std::hash<float>()(Info.DiffuseColor.Z));
        HashCombine(Seed, std::hash<float>()(Info.SpecularColor.X));
        HashCombine(Seed, std::hash<float>()(Info.SpecularColor.Y));
        HashCombine(Seed, std::hash<float>()(Info.SpecularColor.Z));
        HashCombine(Seed, std::hash<float>()(Info.AmbientColor.X));
        HashCombine(Seed, std::hash<float>()(Info.AmbientColor.Y));
        HashCombine(Seed, std::hash<float>()(Info.AmbientColor.Z));
        HashCombine(Seed, std::hash<float>()(Info.EmissiveColor.X));
        HashCombine(Seed, std::hash<float>()(Info.EmissiveColor.Y));
        HashCombine(Seed, std::hash<float>()(Info.EmissiveColor.Z));
        HashCombine(Seed, std::hash<float>()(Info.Shininess));
        HashCombine(Seed, std::hash<float>()(Info.IOR));
        HashCombine(Seed, std::hash<float>()(Info.Transparency));
        HashCombine(Seed, std::hash<float>()(Info.BumpMultiplier));
        HashCombine(Seed, std::hash<uint32_t>()(Info.IlluminanceModel));
        HashCombine(Seed, std::hash<float>()(Info.Metallic));
        HashCombine(Seed, std::hash<float>()(Info.Roughness));
        // TextureInfos는 크기만 반영 (필요시 각 요소 해시 추가)
        for (const FTextureInfo& TexInfo : Info.TextureInfos)
        {
            HashCombine(Seed, std::hash<FWString>()(TexInfo.TexturePath));
            HashCombine(Seed, std::hash<bool>()(TexInfo.bIsSRGB));
        }
        return Seed;
    }
};
