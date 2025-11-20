#pragma once

#include "Launch/Define.h"
#include "HAL/PlatformType.h"
#include "Container/Array.h"
#include "Core/Math/Vector.h"
#include "Classes/Components/Material/MaterialInfo.h"
#include "StaticMeshAsset.h"

struct FSkeletalMeshVertex : public FStaticMeshVertex
{
    uint32 BoneIndices[4] = { 0, 0, 0, 0 };
    float BoneWeights[4] = { 0.f, 0.f, 0.f, 0.f };

    friend FArchive& operator<<(FArchive& Ar, FSkeletalMeshVertex& Data)
    {
        return Ar << Data.X << Data.Y << Data.Z
                  << Data.R << Data.G << Data.B << Data.A
                  << Data.NormalX << Data.NormalY << Data.NormalZ
                  << Data.TangentX << Data.TangentY << Data.TangentZ << Data.TangentW
                  << Data.U << Data.V
                  << Data.BoneIndices[0] << Data.BoneIndices[1] << Data.BoneIndices[2] << Data.BoneIndices[3]
                  << Data.BoneWeights[0] << Data.BoneWeights[1] << Data.BoneWeights[2] << Data.BoneWeights[3];
    }
};

struct FSkeletalMeshRenderData
{
    FWString ObjectName;
    FString DisplayName;

    TArray<FSkeletalMeshVertex> Vertices;
    TArray<uint16> Indices;

    TArray<FMaterialInfo> Materials;
    TArray<FMaterialSubset> MaterialSubsets;

    FVector BoundingBoxMin;
    FVector BoundingBoxMax;

    void Serialize(FArchive& Ar)
    {
        FString ObjectNameStr;
        if (!ObjectName.empty())
        {
            ObjectNameStr = ObjectName;
        }

        Ar << ObjectNameStr
           << DisplayName
           << Vertices
           << Indices
           << Materials
           << MaterialSubsets
           << BoundingBoxMin
           << BoundingBoxMax;

        ObjectName = ObjectNameStr.ToWideString();
    }
};
