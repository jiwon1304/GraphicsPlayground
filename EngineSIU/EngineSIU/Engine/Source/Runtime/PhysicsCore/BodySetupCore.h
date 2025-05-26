#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"

namespace EBodyCollisionResponse
{
    enum class Type : uint8
    {
        BodyCollision_Enabled,
        BodyCollision_Disabled,
    };
};


class UBodySetupCore : public UObject
{
    DECLARE_CLASS(UBodySetupCore, UObject)

public:
    UBodySetupCore() = default;

    UPROPERTY(
        EditAnywhere,
        FName,
        BoneName,
        )

    UPROPERTY(
        EditAnywhere,
        EBodyCollisionResponse::Type,
        CollisionResponse,
        = EBodyCollisionResponse::Type::BodyCollision_Enabled
        )
};
