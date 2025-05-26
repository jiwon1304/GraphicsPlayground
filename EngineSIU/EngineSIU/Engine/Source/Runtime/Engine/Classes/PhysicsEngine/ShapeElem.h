#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"

namespace EAggCollisionShape
{
    enum Type : int
    {
        Sphere,
        Box,
        Sphyl,
        Convex,
        TaperedCapsule,
        LevelSet,
        SkinnedLevelSet,

        Unknown
    };
}


struct FKShapeElem
{
    DECLARE_STRUCT(FKShapeElem)

    /** User-defined name for this shape */
    UPROPERTY(
        EditAnywhere,
        FName,
        Name,
    )

    EAggCollisionShape::Type ShapeType;

    ECollisionEnabled::Type CollisionEnabled = ECollisionEnabled::Type::PhysicsOnly;
};

struct FKBoxElem : public FKShapeElem
{
    DECLARE_STRUCT(FKBoxElem, FKShapeElem)

    FKBoxElem() = default;

    UPROPERTY(
        EditAnywhere,
        FVector,
        Center,
        )

    UPROPERTY(
        EditAnywhere,
        FRotator,
        Rotation,
        )

    // x축 방향으로의 박스의 크기
    // half-extent로 사용됨
    UPROPERTY(
        EditAnywhere,
        float,
        X,
        = 1.f
        )

    // y축 방향으로의 박스의 크기
    // half-extent로 사용됨
    UPROPERTY(
        EditAnywhere,
        float,
        Y,
        = 1.f
        )

    // z축 방향으로의 박스의 크기
    // half-extent로 사용됨
    UPROPERTY(
        EditAnywhere,
        float,
        Z,
        = 1.f
        )
};

struct FKSphereElem : public FKShapeElem
{
    DECLARE_STRUCT(FKSphereElem, FKShapeElem)

    FKSphereElem() = default;

    UPROPERTY(
        EditAnywhere,
        FVector,
        Center,
        )

    // 반지름
    UPROPERTY(
        EditAnywhere,
        float,
        Radius,
        = 1.f
        )
};

// 길이 = 2 * Radius + Length
struct FKSphylElem : public FKShapeElem
{
    DECLARE_STRUCT(FKSphylElem, FKShapeElem)

    FKSphylElem() = default;

    UPROPERTY(
        EditAnywhere,
        FVector,
        Center,
        = FVector::ZeroVector
        )

    UPROPERTY(
        EditAnywhere,
        FRotator,
        Rotation,
        = FRotator::ZeroRotator
        )

    // 반지름
    UPROPERTY(
        EditAnywhere,
        float,
        Radius,
        = 1.f
        )

    // 길이. 양끝 구의 중심 사이의 거리
    UPROPERTY(
        EditAnywhere,
        float,
        Length,
        = 1.f
        )
};
