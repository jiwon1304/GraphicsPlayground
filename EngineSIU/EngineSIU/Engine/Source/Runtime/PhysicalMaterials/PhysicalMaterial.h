#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"

class UPhysicalMaterial : public UObject
{
    DECLARE_CLASS(UPhysicalMaterial, UObject)

public:
    UPhysicalMaterial()
        : Friction(0.5f)
        , StaticFriction(0.5f)
        , Restitution(0.5f)
        , Density(1.0f)
        , BaseFrictionImpulse(0.5f)
    {
    }

    UPROPERTY(
        EditAnywhere,
        float,
        Density,
        = 1.0f
    )

    UPROPERTY(
        EditAnywhere,
        float,
        Friction,
        = 0.5f
    )

    UPROPERTY(
        EditAnywhere,
        float,
        StaticFriction,
        = 0.5f
    )

    // 	A friction (positional) impulse of at least this magnitude may be applied, regardless the normal force.
    UPROPERTY(
        EditAnywhere,
        float,
        BaseFrictionImpulse,
        = 0.5f
    )
    
    // 물리 엔진에서 사용
    struct FPhysicsMaterial* Material;

    // 튕기는 정도. 0일경우 튕기지 않음. 1일 경우 충돌 시 동일한 속도로 튕김
    UPROPERTY(
        EditAnywhere,
        float,
        Restitution,
        = 0.5f
    )
};
