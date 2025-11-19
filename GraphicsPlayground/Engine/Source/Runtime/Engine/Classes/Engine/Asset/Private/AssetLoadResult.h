#pragma once

#include "Core/HAL/PlatformType.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"

struct FLoadResult
{
    /**
     * Path of the file that the asset is loaded from
     * Entire directory path + file name + extension
     */
    FWString FilePath;
    /**
     * Name of the asset.
     * This can be FileNameNoExt if the name is not specified in the file.
     */
    FWString AssetName;
};

// Use move sematics to avoid copying large data
struct FImageLoadResult : public FLoadResult
{
    uint32 Width = 0;
    uint32 Height = 0;
    uint8 NumChannels = 0;
    uint8 BitsPerChannel = 0; // 8bit image -> 8, 32bit image -> 32
    bool bSRGB = false;

    /**
     * Raw image data
     * The channel is interleaved format (e.g., RGBA RGBA RGBA...)
     */
    uint8* ImageData; 
};

struct FMaterialLoadResult : public FLoadResult
{
    // Material name is assigned to AssetName in FLoadResult

    bool bTransparent = false;

    FVector DiffuseColor;   // Kd: Diffuse Color
    FVector SpecularColor;  // Ks: Specular Color
    FVector AmbientColor;   // Ka: Ambient Color
    FVector EmissiveColor;  // Ke: Emissive Color

    float Shininess = 100.f;    // Ns: Specular Power
    float IOR = 1.5f;           // Ni: Index of Refraction
    float Transparency = 0.f;   // d or Tr: Transparency of surface
    float BumpMultiplier = 1.f; // -bm: Bump Multiplier
    uint32 IlluminanceModel;    // illum: illumination Model between 0 and 10.

    float Metallic = 0.0f;      // Pm: Metallic
    float Roughness = 0.5f;     // Pr: Roughness
};