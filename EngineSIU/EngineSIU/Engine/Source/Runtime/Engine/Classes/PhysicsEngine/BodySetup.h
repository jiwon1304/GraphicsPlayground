#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"
#include "AggregateGeom.h"
#include "BodyInstance.h"
#include "PhysicsCore/BodySetupCore.h"

// asset에 들어있는 모든 collision 정보를 담고 있습니다.
// BodyInstance와 같은 physics state를 초기화할 때 사용합니다.
class UBodySetup : public UBodySetupCore
{
    DECLARE_CLASS(UBodySetup, UBodySetupCore)
public:
    UBodySetup() = default;

    // 실제 collision shape을 정의합니다.
    UPROPERTY(
        EditAnywhere,
        FKAggregateGeom,
        AggGeom,
    )

    UPROPERTY(
        EditAnywhere,
        FBodyInstance,
        DefaultInstance,
        {}
    )

    UPROPERTY(
        EditAnywhere,
        class UPhysicalMaterial*,
        PhysMaterial,
        = nullptr
    )

    // static mesh에서의 scale
    UPROPERTY(
        EditAnywhere,
        FVector,
        BuildScale3D,
        = FVector::OneVector
    )
};
