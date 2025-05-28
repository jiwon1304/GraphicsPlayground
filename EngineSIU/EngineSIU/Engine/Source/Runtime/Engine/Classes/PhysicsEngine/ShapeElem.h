#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Math/Shapes.h"

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


inline FArchive& operator<<(FArchive& Ar, EAggCollisionShape::Type& Value)
{
    int Temp = static_cast<int>(Value);

    Ar << Temp;

    if (Ar.IsLoading())
    {
        Value = static_cast<EAggCollisionShape::Type>(Temp);
    }

    return Ar;
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

    FKBoxElem()
    {
        ShapeType = EAggCollisionShape::Type::Box;
    }

    void SetTransform( const FTransform& InTransform )
    {
        assert(InTransform.IsValid());
        Rotation = InTransform.Rotator();
        Center = InTransform.GetTranslation();
    }
    
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

    Shape::FOrientedBox ToFOrientedBox() const
    {
        Shape::FOrientedBox OrientedBox;
        OrientedBox.Center = Center;
        OrientedBox.AxisX = Rotation.RotateVector(FVector(1, 0, 0));
        OrientedBox.AxisY = Rotation.RotateVector(FVector(0, 1, 0));
        OrientedBox.AxisZ = Rotation.RotateVector(FVector(0, 0, 1));
        OrientedBox.ExtentX = X;
        OrientedBox.ExtentY = Y;
        OrientedBox.ExtentZ = Z;
        return OrientedBox;
    }

    inline friend FArchive& operator<<(FArchive& Ar, FKBoxElem& BoxElem)
    {
        Ar << BoxElem.Name << BoxElem.ShapeType;

        uint8 Temp = static_cast<uint8>(BoxElem.CollisionEnabled);

        Ar << Temp;

        if (Ar.IsLoading())
        {
            BoxElem.CollisionEnabled = static_cast<ECollisionEnabled::Type>(Temp);
        }
        
        Ar << BoxElem.Center << BoxElem.Rotation << BoxElem.X << BoxElem.Y << BoxElem.Z;

        return Ar;
    }
};

struct FKSphereElem : public FKShapeElem
{
    DECLARE_STRUCT(FKSphereElem, FKShapeElem)

    FKSphereElem()
    {
        ShapeType = EAggCollisionShape::Type::Sphere;
    }

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

    Shape::FSphere ToFSphere() const
    {
        Shape::FSphere Sphere;
        Sphere.Center = Center;
        Sphere.Radius = Radius;
        return Sphere;
    }

    inline friend FArchive& operator<<(FArchive& Ar, FKSphereElem& SphereElem)
    {
        Ar << SphereElem.Name << SphereElem.ShapeType;

        uint8 Temp = static_cast<uint8>(SphereElem.CollisionEnabled);

        Ar << Temp;

        if (Ar.IsLoading())
        {
            SphereElem.CollisionEnabled = static_cast<ECollisionEnabled::Type>(Temp);
        }
        
        Ar << SphereElem.Center << SphereElem.Radius;

        return Ar;
    }
};

// 길이 = 2 * Radius + Length
struct FKSphylElem : public FKShapeElem
{
    DECLARE_STRUCT(FKSphylElem, FKShapeElem)

    FKSphylElem()
    {
        ShapeType = EAggCollisionShape::Type::Sphyl;
    }

    void SetTransform( const FTransform& InTransform )
    {
        assert(InTransform.IsValid());
        Rotation = InTransform.Rotator();
        Center = InTransform.GetTranslation();
    }
    
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

    Shape::FCapsule ToFCapsule() const
    {
        // 캡슐의 축 방향 (로컬 Y축 기준, Unreal은 Y축이 up이 아님에 유의!)
        // Unreal의 FKSphylElem은 로컬 Z축이 캡슐의 길이 방향임 (참고: 소스 주석)
        // [참고: UE 소스](https://github.com/EpicGames/UnrealEngine/blob/main/Engine/Source/Runtime/Engine/Classes/PhysicsEngine/KSphylElem.h)
        FVector LocalAxis = FVector(0, 0, 1); // Z축 기준
        FVector WorldAxis = Rotation.RotateVector(LocalAxis);

        FVector HalfSegment = WorldAxis * (Length * 0.5f);

        Shape::FCapsule Capsule;
        Capsule.A = Center - HalfSegment;
        Capsule.B = Center + HalfSegment;
        Capsule.Radius = Radius;
        return Capsule;
    }
    
    inline friend FArchive& operator<<(FArchive& Ar, FKSphylElem& SphylElem)
    {
        Ar << SphylElem.Name << SphylElem.ShapeType;

        uint8 Temp = static_cast<uint8>(SphylElem.CollisionEnabled);

        Ar << Temp;

        if (Ar.IsLoading())
        {
            SphylElem.CollisionEnabled = static_cast<ECollisionEnabled::Type>(Temp);
        }
        
        Ar << SphylElem.Center << SphylElem.Rotation << SphylElem.Radius << SphylElem.Length;

        return Ar;
    }
};
