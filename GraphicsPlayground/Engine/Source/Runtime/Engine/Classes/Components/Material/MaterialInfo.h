#pragma once

#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"
#include "Runtime/Launch/Define.h"
#include "Core/Serialization/Archive.h"

struct FMaterialInfo
{
    FString MaterialName;  // newmtl: Material Name.

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
