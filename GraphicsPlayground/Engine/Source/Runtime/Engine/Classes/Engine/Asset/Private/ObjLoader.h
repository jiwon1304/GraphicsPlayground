#pragma once

#include "Launch/EngineLoop.h"
#include "Container/Map.h"
#include "HAL/PlatformType.h"
#include "Serialization/Serializer.h"
#include "Engine/Classes/Engine/Asset/StaticMeshAsset.h"
#include "Engine/Classes/Engine/Asset/AssetInfo.h"

struct FMaterialInfo;
struct FObjInfo;

struct FStaticMeshVertex;
struct FStaticMeshRenderData;

struct FObjLoader
{
public:
    FStaticMeshRenderData* Load(const FString& FilePath);

private:
    // Obj Parsing (*.obj to FObjInfo)
    bool ParseObj(const FString& ObjFilePath, FObjInfo& OutObjInfo);

    // Material Parsing (*.obj to MaterialInfo)
    bool ParseMaterial(FObjInfo& OutObjInfo, FStaticMeshRenderData& OutStaticMeshRenderData);

    // Convert the Raw data to Cooked data (FStaticMeshRenderData)
    bool ConvertToStaticMesh(const FObjInfo& RawData, FStaticMeshRenderData& OutStaticMesh);

    bool CreateTextureFromFile(const FWString& Filename, bool bIsSRGB = true);

    void ComputeBoundingBox(const TArray<FStaticMeshVertex>& InVertices, FVector& OutMinVector, FVector& OutMaxVector);

    void CalculateTangent(FStaticMeshVertex& PivotVertex, const FStaticMeshVertex& Vertex1, const FStaticMeshVertex& Vertex2);

    void CombineMaterialIndex(FStaticMeshRenderData& OutFStaticMesh);

    bool SaveStaticMeshToBinary(const FWString& FilePath, const FStaticMeshRenderData& StaticMesh);

    FStaticMeshRenderData* LoadStaticMeshFromBinary(const FWString& FilePath);
};
























    // Checks whether the same name exists in the cache.
    // If the name exists but the content is different, a warning is logged.
    // @return Pointer to the cached material.
    // Delete argument if the caching failed. (arg and return value are different)
    //FMaterialInfo* FindOrCacheMaterial(const FString& MaterialName, FMaterialInfo* MaterialInfo);

    //UMaterial* CreateMaterial(const FMaterialInfo& MaterialInfo);

    //TMap<FString, FStaticMeshRenderData*> CachedObjStaticMeshMap;

    //TMap<FString, FMaterialInfo*> CachedMaterialMap;
//
//
//struct FObjManager
//{
//public:
//    static FStaticMeshRenderData* LoadObjStaticMeshAsset(const FString& PathFileName);
//
//    static TMap<FString, UMaterial*>& GetMaterials() { return MaterialMap; }
//
//    static UMaterial* GetMaterial(const FString& Name);
//
//    static int GetMaterialNum() { return MaterialMap.Num(); }
//
//    static UStaticMesh* CreateStaticMesh(const FString& FilePath);
//
//    static const TMap<FWString, UStaticMesh*>& GetStaticMeshes() { return StaticMeshMap; }
//
//    static UStaticMesh* GetStaticMesh(const FWString& Name);
//
//    static int GetStaticMeshNum() { return StaticMeshMap.Num(); }
//
//private:
//    inline static TMap<FString, FStaticMeshRenderData*> ObjStaticMeshMap;
//    inline static TMap<FWString, UStaticMesh*> StaticMeshMap;
//    inline static TMap<FString, UMaterial*> MaterialMap;
//};
