#pragma once

#include "Engine/Classes/GameFramework/Pawn.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "VehicleMovementComponent.h"


class AWheeledVehiclePawn : public APawn 
{
    DECLARE_CLASS(AWheeledVehiclePawn, APawn)

public:
    AWheeledVehiclePawn() = default;

private:
    USkeletalMeshComponent* Mesh;
    UVehicleMovementComponent* VehicleMovementComponent;

    // TODO 아래 UPROPERTY 가능하도록 작업하기
    /*UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        USkeletalMeshComponent, Mesh
    )
    
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        UVehicleMovementComponent, VehicleMovementComponent
    )*/

public:
    void SetMesh(USkeletalMeshComponent* InSkeletalMeshComponent) 
    { 
        Mesh = InSkeletalMeshComponent;
    }

    USkeletalMeshComponent* GetMesh() const { return Mesh; }

    void SetVehicleMovementComponent(UVehicleMovementComponent* InVehicleMovementComponent) 
    { 
        VehicleMovementComponent = InVehicleMovementComponent;
    }
    UVehicleMovementComponent* GetVehicleMovementComponent() const { return VehicleMovementComponent; }
};
