#pragma once

#include "Defines.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include "UObject/ObjectFactory.h"

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
    struct FPhysicsMaterial* Material = nullptr;

    // 튕기는 정도. 0일경우 튕기지 않음. 1일 경우 충돌 시 동일한 속도로 튕김
    UPROPERTY(
        EditAnywhere,
        float,
        Restitution,
        = 0.5f
    )

    friend FArchive& operator<<(FArchive& Ar, UPhysicalMaterial*& PhysicalMaterial)
    {
        if (Ar.IsLoading())
        {
            if (PhysicalMaterial == nullptr)
            {
                PhysicalMaterial = FObjectFactory::ConstructObject<UPhysicalMaterial>(nullptr);
            }
            if (PhysicalMaterial->Material == nullptr)
            {
                PhysicalMaterial->Material = new FPhysicsMaterial();
            }
        }
        
        Ar << PhysicalMaterial->Density
            << PhysicalMaterial->Friction
            << PhysicalMaterial->StaticFriction
            << PhysicalMaterial->BaseFrictionImpulse
            << *PhysicalMaterial->Material
            << PhysicalMaterial->Restitution;
        return Ar;
    }

};
