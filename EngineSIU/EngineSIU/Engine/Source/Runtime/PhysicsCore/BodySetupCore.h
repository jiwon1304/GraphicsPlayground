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

inline FArchive& operator<<(FArchive& Ar, EBodyCollisionResponse::Type& Value)
{
    int8 Temp = static_cast<int8>(Value);

    Ar << Temp;

    if (Ar.IsLoading())
    {
        Value = static_cast<EBodyCollisionResponse::Type>(Temp);
    }

    return Ar;
}

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

    inline friend FArchive& operator<<(FArchive& Ar, UBodySetupCore& BodySetupCore)
    {
        return Ar << BodySetupCore.BoneName << BodySetupCore.CollisionResponse;
    }
};
