#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "PhysicsCore/PhysicsScene.h"
#include "ConstraintTypes.h"

struct FConstraintProfileProperties
{
    DECLARE_STRUCT(FConstraintProfileProperties)

    // 선속도
    UPROPERTY(
        EditAnywhere,
        FLinearConstraint,
        LinearLimit,
    )

    // 각속도
    UPROPERTY(
        EditAnywhere,
        FConeConstraint,
        ConeLimit,
        )
    
    // 이 constraint애 연결된 body와의 충돌 활성화 여부
    UPROPERTY(
        EditAnywhere,
        uint8,
        bDisableCollision,
        )
};






struct FConstraintInstance
{
    DECLARE_STRUCT(FConstraintInstance)

    // USkeletalMeshComponent에서의 index
    int32 ConstraintIndex;
    
    // 현재 속해있는 물리 씬
    FPhysScene* PhysicsScene;

    UPROPERTY(
        VisibleAnywhere,
        FName,
        JointName,
    )

    // 해당 Joint가 연결되어있는 child bone
    UPROPERTY(
        EditAnywhere,
        FName,
        ConstraintBone1,
    )

    // 해당 Joint가 연결되어있는 parent bone
    UPROPERTY(
        EditAnywhere,
        FName,
        ConstraintBone2,
    )

    ///////////////////////////// Body1 ref frame

    /** Location of constraint in Body1 reference frame (usually the "child" body for skeletal meshes). */
    UPROPERTY(
        EditAnywhere,
        FVector,
        Pos1,
    )

    /** Primary (twist) axis in Body1 reference frame. */
    UPROPERTY(
        EditAnywhere,
        FVector,
        PriAxis1,
    )

    /** Secondary axis in Body1 reference frame. Orthogonal to PriAxis1. */
    UPROPERTY(
        EditAnywhere,
        FVector,
        SecAxis1,
    )
    ///////////////////////////// Body2 ref frame

    /** Location of constraint in Body2 reference frame (usually the "parent" body for skeletal meshes). */
    UPROPERTY(
        EditAnywhere,
        FVector,
        Pos2,
    )

    /** Primary (twist) axis in Body2 reference frame. */
    UPROPERTY(
        EditAnywhere,
        FVector,
        PriAxis2,
    )

    /** Secondary axis in Body2 reference frame. Orthogonal to PriAxis2. */
    UPROPERTY(
        EditAnywhere,
        FVector,
        SecAxis2,
    )
};
