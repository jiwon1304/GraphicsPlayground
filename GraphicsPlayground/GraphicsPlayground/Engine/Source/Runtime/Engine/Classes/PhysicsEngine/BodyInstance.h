#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Engine/PhysxUserData.h"
#include "EngineTypes.h"
//#include "ConstraintInstance.h"
#include "Engine/EngineTypes.h"

struct FShapeData;

namespace EDOFMode
{
    enum class Type : int8
    {
        /*Inherits the degrees of freedom from the project settings.*/
        Default,
        /*Specifies which axis to freeze rotation and movement along.*/
        SixDOF,
        /*Allows 2D movement along the Y-Z plane.*/
        YZPlane,
        /*Allows 2D movement along the X-Z plane.*/
        XZPlane,
        /*Allows 2D movement along the X-Y plane.*/
        XYPlane,
        /*Allows 2D movement along the plane of a given normal*/
        CustomPlane,
        /*No constraints.*/
        None
    };
}

inline FArchive& operator<<(FArchive& Ar, EDOFMode::Type& Value)
{
    int8 Temp = static_cast<int8>(Value);

    Ar << Temp;

    if (Ar.IsLoading())
    {
        Value = static_cast<EDOFMode::Type>(Temp);
    }

    return Ar;
}

inline FArchive& operator<<(FArchive& Ar, ECollisionChannel& Value)
{
    uint8 Temp = static_cast<uint8>(Value);

    Ar << Temp;

    if (Ar.IsLoading())
    {
        Value = static_cast<ECollisionChannel>(Temp);
    }

    return Ar;
}

// 물체의 물리적 특성을 정의하는 구조체
struct FBodyInstance
{
    DECLARE_STRUCT(FBodyInstance)

    // simulation을 사용할 것인지의 여부
    // false일 경우 움직이지 않음
    UPROPERTY(
        EditAnywhere,
        bool,
        bSimulatePhysics,
        = true
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bEnableGravity,
        = true
    )

    // PhysX 에서 나온 값이 고정적인 회전이 필요한 경우 아래 매트릭스 활용
    UPROPERTY(
        EditAnywhere,
        FQuat,
        InvPhysXQuat,
        = FQuat::Identity
    )

    // Car이면 아래 값 true
    UPROPERTY(
        EditAnywhere,
        bool,
        bCar,
        = false
    )

    // SkeletalMeshComponent / PhysicsAsset에서의 index
    int32 InstanceBodyIndex;

    // SkeletalMeshComponent에서의 bone index
    int16 InstanceBoneIndex;

    // 움직일 때 해당 물체가 어떻게 처리되어야하는지
    UPROPERTY(
        EditAnywhere,
        ECollisionChannel,
        ObjectType,
        = ECollisionChannel::ECC_WorldStatic
    )

    // 충돌할 때 어떻게 처리되어야하는지
    UPROPERTY(
        EditAnywhere,
        ECollisionEnabled::Type,
        CollisionEnabled,
        = ECollisionEnabled::Type::PhysicsOnly
    )

public:
    // 특정 축을 고정할지 여부
    UPROPERTY(
        EditAnywhere,
        EDOFMode::Type,
        DOFMode,
        = EDOFMode::Type::Default
    )

    // Conitnuous Collision Detection
    UPROPERTY(
        EditAnywhere,
        uint8,
        bUseCCD,
        = 1;
    )

public:
    // Physx에는 sacle이 없기 때문에...
    FVector Scale3D = FVector::OneVector;

    // linear movement에 대한 마찰력 / 항력
    UPROPERTY(
        EditAnywhere,
        float,
        LinearDamping,
        = 0.0f
    )

    // angular movement에 대한 마찰력 / 항력
    UPROPERTY(
        EditAnywhere,
        float,
        AngularDamping,
        = 0.0f
    )

    UPROPERTY(        
        EditAnywhere,
        float,
        MassScale,
        = 1.f
    )
    // physx에서 사용할 Constraint
    struct FConstraintInstance* DOFConstraint;

    // 현재 body가 붙어있는 parent body
    FBodyInstance* WeldParent;

    // UPROPETY에서 bit는 못사용할듯...
    UPROPERTY(
        EditAnywhere,
        bool,
        bLockXTranslation,
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bLockYTranslation,
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bLockZTranslation,
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bLockRotation,
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bLockXRotation,
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bLockYRotation,
    )

    UPROPERTY(
        EditAnywhere,
        bool,
        bLockZRotation,
    )
    
    UPROPERTY(
        EditAnywhere,
        bool,
        bNotifyRigidBodyCollision,
    )
    
    // 이 물체를 가지고 있는 PrimitiveComponent (필요시)
    class UPrimitiveComponent* OwnerComponent = nullptr;

    // 이 물체를 가지고 있는 UObject (보통은 OwnerComponent의 Owner)
    class UObject* OwnerObject = nullptr;

    // default body instance와 collision profile을 가진 셋업
    // 물체의 모양이 여기 들어있음
    class UBodySetup* ExternalCollisionProfileBodySetup = nullptr;

    friend FArchive& operator<<(FArchive& Ar, FBodyInstance& BodyInstance)
    {        
        Ar << BodyInstance.bSimulatePhysics
            << BodyInstance.bEnableGravity
            << BodyInstance.InstanceBodyIndex
            << BodyInstance.InstanceBoneIndex
            << BodyInstance.ObjectType;

        uint8 Temp = static_cast<uint8>(BodyInstance.CollisionEnabled);

        Ar << Temp;

        if (Ar.IsLoading())
        {
            BodyInstance.CollisionEnabled = static_cast<ECollisionEnabled::Type>(Temp);
        }

        Ar << BodyInstance.DOFMode << BodyInstance.bUseCCD << BodyInstance.Scale3D << BodyInstance.LinearDamping
                << BodyInstance.AngularDamping << BodyInstance.MassScale << BodyInstance.bLockXTranslation
                << BodyInstance.bLockYTranslation << BodyInstance.bLockZTranslation << BodyInstance.bLockRotation << BodyInstance.bLockXRotation << BodyInstance.bLockYRotation
                << BodyInstance.bLockZRotation << BodyInstance.bNotifyRigidBodyCollision;
        
        return Ar;
        
    }
};
