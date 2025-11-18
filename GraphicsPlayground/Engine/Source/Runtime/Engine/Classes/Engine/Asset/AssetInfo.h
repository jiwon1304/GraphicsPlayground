#pragma once

#include "Core/Container/String.h"
#include "CoreUObject/UObject/NameTypes.h"
#include "Core/Serialization/Archive.h"


enum class EAssetType : uint8
{
    StaticMesh,
    SkeletalMesh,
    Skeleton,
    Animation,
    Texture2D,
    Material,
    PhysicsAsset,
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
    FName AssetName;            // Name of asset, without path and extension
    FString AssetPath;          // Full path of asset including name with extension
    EAssetType AssetType;       // Type of asset
    uint32 Size;                // Size of asset in bytes
    ELoadState LoadState;       // Current load state of the asset
    void* Resource = nullptr;   // Pointer to the loaded resource (UObject derived class)

    void Serialize(FArchive& Ar)
    {
        int8 Type = static_cast<int8>(AssetType);

        Ar << AssetName
           << AssetPath
           << Type
           << Size;

        AssetType = static_cast<EAssetType>(Type);
    }
};
