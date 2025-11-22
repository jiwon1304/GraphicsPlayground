#pragma once
#include "Define.h"
#include "Hal/PlatformType.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quat.h"
#include "Math/Color.h"
#include "Math/Transform.h"
#include "StaticMeshAsset.h"

struct FMeshBoneInfo
{
    FString Name;
    int32 ParentIndex;
};

// inverse bind pose는 USkeletalMesh에 존재.
struct FReferenceSkeleton
{
    // TODO : RawRefBonePose을 행렬로 캐싱하기
    TArray<FMeshBoneInfo> RawRefBoneInfo;
    // joint pose 저장용도. Index는 RawRefBoneInfo를 따라갑니다.
    TArray<FTransform> RawRefBonePose;
    TMap<FString, int32> RawNameToIndexMap;

    int32 GetRawBoneNum() const
    {
        return RawRefBoneInfo.Num();
    }
};  

// !!! FFbxVertex랑 메모리 레이아웃이 같아야합니다.
struct FSkeletalMeshVertex : public FStaticMeshVertex
{
    int BoneIndices[8];
    float BoneWeights[8];

    inline const static D3D11_INPUT_ELEMENT_DESC LayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_INDICES", 1, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_WEIGHTS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    }; 
};

struct FSkelMeshRenderSection
{
    TArray<FSkeletalVertex> Vertices;
    TArray<uint32> Indices;
    TArray<uint32> SubsetIndex;
    //TArray<uint8> BoneIndices;
    //TArray<float> BoneWeights;
    FString Name;
};

// 수정되지 않는 데이터입니다.
// 포즈를 수정하려면 USkeletalMesh에 있는 RefSkeleton을 수정해야합니다.
struct FSkeletalMeshRenderData
{
    FString ObjectName;
    //FString DisplayName;

    // Render Data
    //TArray<FVector> Vertices;
    //TArray<FLinearColor> Colors;
    //TArray<FVector> Normals;
    //TArray<FVector> Tangents;
    //TArray<FVector2D> UVs;
    //TArray<uint32> MaterialIndices;
    //TArray<uint8> BoneIndices;
    //TArray<float> BoneWeights;

    TArray<FSkelMeshRenderSection> RenderSections;
    TArray<FMaterialSubset> MaterialSubsets;
};


// #pragma once

// #include "Launch/Define.h"
// #include "HAL/PlatformType.h"
// #include "Container/Array.h"
// #include "Core/Math/Vector.h"
// #include "Classes/Components/Material/MaterialInfo.h"
// #include "StaticMeshAsset.h"

// struct FSkeletalMeshVertex : public FStaticMeshVertex
// {
//     uint32 BoneIndices[4] = { 0, 0, 0, 0 };
//     float BoneWeights[4] = { 0.f, 0.f, 0.f, 0.f };

//     friend FArchive& operator<<(FArchive& Ar, FSkeletalMeshVertex& Data)
//     {
//         return Ar << Data.X << Data.Y << Data.Z
//                   << Data.R << Data.G << Data.B << Data.A
//                   << Data.NormalX << Data.NormalY << Data.NormalZ
//                   << Data.TangentX << Data.TangentY << Data.TangentZ << Data.TangentW
//                   << Data.U << Data.V
//                   << Data.BoneIndices[0] << Data.BoneIndices[1] << Data.BoneIndices[2] << Data.BoneIndices[3]
//                   << Data.BoneWeights[0] << Data.BoneWeights[1] << Data.BoneWeights[2] << Data.BoneWeights[3];
//     }
// };

// struct FSkeletalMeshRenderData
// {
//     FWString ObjectName;
//     FString DisplayName;

//     TArray<FSkeletalMeshVertex> Vertices;
//     TArray<uint16> Indices;

//     TArray<FMaterialInfo> Materials;
//     TArray<FMaterialSubset> MaterialSubsets;

//     FVector BoundingBoxMin;
//     FVector BoundingBoxMax;

//     void Serialize(FArchive& Ar)
//     {
//         FString ObjectNameStr;
//         if (!ObjectName.empty())
//         {
//             ObjectNameStr = ObjectName;
//         }

//         Ar << ObjectNameStr
//            << DisplayName
//            << Vertices
//            << Indices
//            << Materials
//            << MaterialSubsets
//            << BoundingBoxMin
//            << BoundingBoxMax;

//         ObjectName = ObjectNameStr.ToWideString();
//     }
// };
