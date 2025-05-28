#pragma once

#include "Engine/Classes/GameFramework/Pawn.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "VehicleMovementComponent.h"


class AWheeledVehiclePawn : public APawn 
{
    DECLARE_CLASS(AWheeledVehiclePawn, APawn)

public:
    AWheeledVehiclePawn();
    
    virtual void PostSpawnInitialize() override;
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

    USkeletalMeshComponent* GetSkeltalMesh() const;
    UStaticMeshComponent* GetStaticMesh() const;
    UVehicleMovementComponent* GetVehicleMovementComponent() const;
};
