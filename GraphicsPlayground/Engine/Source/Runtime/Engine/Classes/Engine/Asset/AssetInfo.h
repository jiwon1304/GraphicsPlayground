#pragma once

#include "Core/Container/String.h"
#include "CoreUObject/UObject/NameTypes.h"
#include "Core/Serialization/Archive.h"
#include "AssetDefines.h"

enum class EAssetType : uint8
{
    StaticMesh,
    SkeletalMesh,
    Skeleton,
    Animation,
    Texture2D,
    Material,
    PhysicsAsset,
    
    Unknown
};

enum class ELoadState : uint8
{
    Pending,
    Loading,
    Success,
    Failed,
};

struct FAssetInfo
{
    FName AssetName;                // Name of asset, without path and extension
    // Absolute filename + AssetName
    FString InternalName;
    FFilePath AssetPath;
    EAssetType AssetType;           // Type of asset
    uint32 Size;                    // Size of asset in bytes
    ELoadState LoadState;           // Current load state of the asset
    void* NativeResource = nullptr; // Pointer to native resource, starts with F-

    constexpr uint32 MaterialSize = 0;
};
