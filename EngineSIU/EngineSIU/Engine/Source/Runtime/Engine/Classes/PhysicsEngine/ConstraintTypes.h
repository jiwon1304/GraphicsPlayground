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
};
