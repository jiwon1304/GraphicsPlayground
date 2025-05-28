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
    virtual UObject* Duplicate(UObject* InOuter) override;
    void BeginPlay() override;
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

    FVector ForwardLeftTireLocation = FVector(16.05f, -8.44f, 2.29f);
    FVector ForwardRightTireLocation = FVector(16.05f, 8.44f, 2.29f);
    FVector RearLeftTireLocation = FVector(-15.80f, -8.69f, 2.81f);
    FVector RearRightTireLocation = FVector(-15.80f, 8.69f, 2.81f);

    USkeletalMeshComponent* GetSkeltalMesh() const;
    UStaticMeshComponent* GetStaticMesh() const;
    UVehicleMovementComponent* GetVehicleMovementComponent() const;
};
