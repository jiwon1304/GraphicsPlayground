#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"

// Linear DOF
enum class ELinearConstraintMotion : uint8
{
    // 해당 축에 대해서 제한 없음
    LCM_Free,
    // 해당 축에 대해서만 제한이 없음
    LCM_Limited,
    // 해당 축에 대해서만 제한이 있음
    LCM_Locked,
    LCM_MAX
};

// Angular DOF
enum class EAngularConstraintMotion : uint8
{
    // 해당 축에 대해서 제한 없음
    ACM_Free,
    // 해당 축에 대해서만 제한이 없음
    ACM_Limited,
    // 해당 축에 대해서만 제한이 있음
    ACM_Locked,
    ACM_MAX
};

inline FArchive& operator<<(FArchive& Ar, ELinearConstraintMotion& LinearConstraintMotion)
{
    uint8 Temp = static_cast<uint8>(LinearConstraintMotion);

    Ar << Temp;

    if (Ar.IsLoading())
    {
        LinearConstraintMotion = static_cast<ELinearConstraintMotion>(Temp);
    }

    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, EAngularConstraintMotion& LinearConstraintMotion)
{
    uint8 Temp = static_cast<uint8>(LinearConstraintMotion);

    Ar << Temp;

    if (Ar.IsLoading())
    {
        LinearConstraintMotion = static_cast<EAngularConstraintMotion>(Temp);
    }

    return Ar;
}

struct FConstraintBaseParams
{
    DECLARE_STRUCT(FConstraintBaseParams)

    // soft constraint (spring)
    UPROPERTY(
        EditAnywhere,
        uint8,
        bSoftConstraint,
    )

    // joint를 회전시키기 위한 joint와 충돌지점 사이의 최소 거리
    UPROPERTY(
        EditAnywhere,
        float,
        ContactDistance,
    )

    UPROPERTY(
        EditAnywhere,
        float,
        Damping,
    )

    UPROPERTY(
        EditAnywhere,
        float,
        Restitution,
    )

    UPROPERTY(
        EditAnywhere,
        float,
        Stiffness,
        )
    
    friend FArchive& operator<<(FArchive& Ar, FConstraintBaseParams& ConstraintBaseParams)
    {
        return Ar << ConstraintBaseParams.bSoftConstraint << ConstraintBaseParams.ContactDistance << ConstraintBaseParams.Damping << ConstraintBaseParams.Restitution << ConstraintBaseParams.Stiffness;
    }
};

struct FLinearConstraint : public FConstraintBaseParams
{
    DECLARE_STRUCT(FLinearConstraint, FConstraintBaseParams)

    // 두 joint 사이의 거리 제한
    UPROPERTY(
        EditAnywhere,
        float,
        Limit,
    )

    UPROPERTY(
        EditAnywhere,
        ELinearConstraintMotion,
        XMotion,
    )

    UPROPERTY(
        EditAnywhere,
        ELinearConstraintMotion,
        YMotion,
    )

    UPROPERTY(
        EditAnywhere,
        ELinearConstraintMotion,
        ZMotion,
    )
    
    friend FArchive& operator<<(FArchive& Ar, FLinearConstraint& LinearConstraint)
    {
        Ar << static_cast<FConstraintBaseParams&>(LinearConstraint);

        return Ar << LinearConstraint.Limit << LinearConstraint.XMotion << LinearConstraint.YMotion << LinearConstraint.ZMotion;
    }
};

// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/PhysicsEngine/FConeConstraint
struct FConeConstraint : public FConstraintBaseParams
{
    DECLARE_STRUCT(FConeConstraint, FConstraintBaseParams)
    // xy 평면에서의 각도 제한
    UPROPERTY(
        EditAnywhere,
        float,
        Swing1LimitDegrees,
        )
    // Swing1LimitDegrees에 대한 제한
    UPROPERTY(
        EditAnywhere,
        EAngularConstraintMotion,
        Swing1Motion,
        )
    // xz 평면에서의 각도 제한
    UPROPERTY(
        EditAnywhere,
        float,
        Swing2LimitDegrees,
        )
    // Swing2LimitDegrees에 대한 제한
    UPROPERTY(
        EditAnywhere,
        EAngularConstraintMotion,
        Swing2Motion,
        )
        
    friend FArchive& operator<<(FArchive& Ar, FConeConstraint& ConeConstraint)
    {
        Ar << static_cast<FConstraintBaseParams&>(ConeConstraint);

        return Ar << ConeConstraint.Swing1LimitDegrees << ConeConstraint.Swing1Motion << ConeConstraint.Swing2LimitDegrees << ConeConstraint.Swing2Motion;
    }
};


struct FTwistConstraint : public FConstraintBaseParams
{
    DECLARE_STRUCT(FTwistConstraint, FConstraintBaseParams)
    // Twist 축에 대한 제한
    UPROPERTY(
        EditAnywhere,
        float,
        TwistLimitDegrees,
        )
    // Twist 축에 대한 제한
    UPROPERTY(
        EditAnywhere,
        EAngularConstraintMotion,
        TwistMotion,
        )

    friend FArchive& operator<<(FArchive& Ar, FTwistConstraint& TwistConstraint)
    {
        Ar << static_cast<FConstraintBaseParams&>(TwistConstraint);

        return Ar << TwistConstraint.TwistLimitDegrees << TwistConstraint.TwistMotion;
    }
};
