#pragma once
#include "World.h"
#include "PhysicsEngine/ShapeElem.h"


struct FSelectedShape
{
    int32 ParentBodySetupIndex = -1;
    int32 SelectedShapeIndex = -1;

    EAggCollisionShape::Type ShapeType = EAggCollisionShape::Unknown;
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

    // Index가 도중에 변화한 경우 이를 보장하지 않음. (보장하기 위해선 Index가 아닌 FName으로 저장 후 IndexMapping을 이용해야 됨)
    int32 SelectBoneIndex = -1;
    int32 SelectedBodySetupIndex = -1;
    FSelectedShape SelectedShape;

private:
    USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
    
};
