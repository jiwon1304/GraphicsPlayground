#pragma once
#include "World.h"
#include "PhysicsEngine/ShapeElem.h"


struct FSelectedPrimitive
{
    int32 ParentBodySetupIndex = -1;
    int32 SelectedPrimitiveIndex = -1;

    EAggCollisionShape::Type PrimitiveType = EAggCollisionShape::Unknown;

    FSelectedPrimitive() : ParentBodySetupIndex(-1), SelectedPrimitiveIndex(-1), PrimitiveType(EAggCollisionShape::Unknown) {}
};

class UPhysicsAssetWorld : public UWorld
{
    DECLARE_CLASS(UPhysicsAssetWorld, UWorld)

public:
    UPhysicsAssetWorld() = default;
        
    static UPhysicsAssetWorld* CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName = "DefaultWorld");
    virtual void Tick(float DeltaTime) override;

    void SetSkeletalMeshComponent(USkeletalMeshComponent* Component)
    {
        SkeletalMeshComponent = Component;
    }
    USkeletalMeshComponent* GetSkeletalMeshComponent()
    {
        return SkeletalMeshComponent;
    }

    void ClearSelected();

    // Index가 도중에 변화한 경우 이를 보장하지 않음. (보장하기 위해선 Index가 아닌 FName으로 저장 후 IndexMapping을 이용해야 됨)
    // Multiple Select를 구현하거나 하지 않음.
    int32 SelectBoneIndex = -1;
    int32 SelectedBodySetupIndex = -1;
    int32 SelectedConstraintIndex = -1;
    FSelectedPrimitive SelectedPrimitive;


    uint8 bShowBoneIndices : 1 = true;
    uint8 bShowBones : 1 = true;
    uint8 bShowBodies : 1 = true;
    uint8 bShowConstraints : 1 = true;
    uint8 bShowPrimitives : 1 = true;
    
private:
    USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
    
};
