#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"
#include "ConstraintInstance.h"

struct FPhysicsConstraintProfileHandle
{
    DECLARE_STRUCT(FPhysicsConstraintProfileHandle)

    UPROPERTY(
        FConstraintProfileProperties,
        ProfileProperties
    )

    UPROPERTY(
        EditAnywhere,
        FName,
        ProfileName,
    )
    
    friend FArchive& operator<<(FArchive& Ar, FPhysicsConstraintProfileHandle& PhysicsConstraintProfileHandle)
    {
        return Ar << PhysicsConstraintProfileHandle.ProfileProperties << PhysicsConstraintProfileHandle.ProfileName;
    }
};

class UPhysicsConstraintTemplate : public UObject
{
    DECLARE_CLASS(UPhysicsConstraintTemplate, UObject)

public:
    UPhysicsConstraintTemplate() = default;

    // Bone의 위치정보
    UPROPERTY(
        EditAnywhere,
        FConstraintInstance, 
        DefaultInstance,
    )

    // Bone 사이의 constraint 정보
    UPROPERTY(
        TArray<FPhysicsConstraintProfileHandle>,
        ProfileHandles
    )

    friend FArchive& operator<<(FArchive& Ar, UPhysicsConstraintTemplate*& PhysicsConstraintTemplate);
};
