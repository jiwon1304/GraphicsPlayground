#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Components/Material/Material.h"
#include "Define.h"

struct FStaticMeshRenderData;
class UBodySetup;

class UStaticMesh : public UObject
{
    DECLARE_CLASS(UStaticMesh, UObject)

public:
    UStaticMesh() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;

    const TArray<FStaticMaterial*>& GetMaterials() const { return Materials; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& OutMaterial) const;
    FStaticMeshRenderData* GetRenderData() const { return RenderData; }

    //ObjectName은 경로까지 포함
    FWString GetOjbectName() const;

    void SetData(FStaticMeshRenderData* InRenderData);

    virtual void SerializeAsset(FArchive& Ar) override;

    void SetBodySetup(UBodySetup* InBodySetup) { BodySetup = InBodySetup; }
    UBodySetup* GetBodySetup() const { return BodySetup; }

private:
    FStaticMeshRenderData* RenderData = nullptr;
    TArray<FStaticMaterial*> Materials;
    UPROPERTY(
        EditAnywhere | EditInline,
        UBodySetup*,
        BodySetup,
        = nullptr
    )
};
