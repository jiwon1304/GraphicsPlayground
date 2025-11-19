#pragma once

#include <filesystem>
#include "Core/HAL/PlatformType.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"
#include "Classes/Engine/Asset/AssetInfo.h"
#include "Classes/Engine/Asset/StaticMeshAsset.h"
#include "Classes/Engine/Asset/SkeletalMeshAsset.h"

using FFilePath = std::filesystem::path;

/**
 * This struct only has file path for other asset types.
 * The actual data will be loaded after parsing.
 */
struct FLoadResult
{
    /**
     * Path of the file that the asset is loaded from
     * Entire directory path + file name + extension
     * This field is used for lazy mapping of texture/material files.
     */
    FFilePath AbsoluteFilePath;
    /**
     * Name of the asset.
     * This can be FileNameNoExt if the name is not specified in the file.
     */
    FString AssetName;
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
    bool bTransparent = false;

    // Whether the material is validly parsed
    // Used in parsing process.
    bool bValid = false;

    /**
     * Texture files will be resolved after parsing mtl file.
     */
    FFilePath DiffuseTexturePath;    // map_Kd
    FFilePath SpecularTexturePath;   // map_Ks
    FFilePath NormalTexturePath;     // map_Bump
    FFilePath EmissiveTexturePath;   // map_Ke
    FFilePath AlphaTexturePath;      // map_d
    FFilePath AmbientTexturePath;    // map_Ka
    FFilePath ShininessTexturePath;  // map_Ns
    FFilePath MetallicTexturePath;   // map_Pm
    FFilePath RoughnessTexturePath;  // map_Pr
};

struct FSubMeshInfo
{
    // Index for FStaticMeshLoadResult::Vertices
    IndexType IndexStart;
    IndexType IndexCount;
    // Will be resolved after parsing
    FFilePath MaterialPath;
};

struct FMeshLoadResult : public FLoadResult
{
    // Positions only for geometry
    TArray<FVector> PositionOnly;
    // Indices for PositionOnly
    TArray<uint16> PositionOnlyIndices;
};

struct FStaticMeshLoadResult : public FMeshLoadResult
{
    // Full vertex data
    TArray<FStaticMeshVertex> Vertices;    
    TArray<FSubMeshInfo> SubMeshes;
};

struct FSkeletalMeshLoadResult : public FMeshLoadResult
{
    TArray<FSkeletalMeshVertex> Vertices;
    TArray<FSubMeshInfo> SubMeshes;
};