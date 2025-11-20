#pragma once

#include "Launch/Define.h"
#include "HAL/PlatformType.h"
#include "Container/Array.h"
#include "Classes/Components/Material/MaterialInfo.h"

struct FStaticMeshVertex
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
    float NormalX, NormalY, NormalZ;
    float TangentX, TangentY, TangentZ, TangentW;
    float U = 0, V = 0;

    friend FArchive& operator<<(FArchive& Ar, FStaticMeshVertex& Data)
    {
        return Ar << Data.X << Data.Y << Data.Z
                  << Data.R << Data.G << Data.B << Data.A
                  << Data.NormalX << Data.NormalY << Data.NormalZ
                  << Data.TangentX << Data.TangentY << Data.TangentZ << Data.TangentW
                  << Data.U << Data.V;
    }
};

struct FStaticMeshRenderData
{
    FString FileNameNoExtension;
    FString FilePathWithFileFullName;
    //FString DisplayName;
    //FWString FilePath;

    TArray<FStaticMeshVertex> Vertices;
    TArray<uint16> Indices;

    TArray<FMaterialInfo*> Materials;
    TArray<FMaterialSubset> MaterialSubsets;

    FVector BoundingBoxMin;
    FVector BoundingBoxMax;

    void Serialize(FArchive& Ar)
    {
        FString ObjectNameStr = ObjectName;

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
