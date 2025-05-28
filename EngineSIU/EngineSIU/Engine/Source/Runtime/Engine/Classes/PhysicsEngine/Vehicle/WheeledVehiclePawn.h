#pragma once

#include "Engine/Classes/GameFramework/Pawn.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "VehicleMovementComponent.h"


class AWheeledVehiclePawn : public APawn 
{
    DECLARE_CLASS(AWheeledVehiclePawn, APawn)

public:
    AWheeledVehiclePawn();

private:

    // TODO 아래 UPROPERTY 가능하도록 작업하기
    /*UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        USkeletalMeshComponent, MeshComponent
    )
    
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        UVehicleMovementComponent, VehicleMovementComponent
    )*/

public:

    USkeletalMeshComponent* GetMesh() const;
    UVehicleMovementComponent* GetVehicleMovementComponent() const;
};
