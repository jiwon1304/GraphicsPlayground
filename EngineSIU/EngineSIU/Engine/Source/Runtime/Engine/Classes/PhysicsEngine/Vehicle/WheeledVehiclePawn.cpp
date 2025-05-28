#include "WheeledVehiclePawn.h"
#include "Engine/Classes/Engine/AssetManager.h"

AWheeledVehiclePawn::AWheeledVehiclePawn()
{
    USkeletalMeshComponent* MeshComponent = AddComponent<USkeletalMeshComponent>();
    SetRootComponent(MeshComponent);

    MeshComponent->SetSkeletalMeshAsset(UAssetManager::Get().GetSkeletalMesh("Contents/Asset/SportsVehicle"));
    MeshComponent->SetupAttachment(nullptr);
    MeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

    UVehicleMovementComponent* VehicleMovementComponent = AddComponent<UVehicleMovementComponent>();

    FVector CarBox = MeshComponent->AABB.MaxLocation - MeshComponent->AABB.MinLocation;
    CarBox = CarBox * 0.1f;

    VehicleMovementComponent->ChassisBoxExtents = FVector(CarBox.X, CarBox.Z, CarBox.Y);
    
    VehicleMovementComponent->WheelRadius = CarBox.Y / 5.5f;
    VehicleMovementComponent->WheelWidth = CarBox.X / 4.5f;

    VehicleMovementComponent->FrontLeftWheelOffset = FVector(-1.049995f, -1.0f, 1.5f) * CarBox.Y / 4.f;
    VehicleMovementComponent->FrontRightWheelOffset = FVector(1.049995f, -1.0f, 1.5f) * CarBox.Y / 4.f;
    VehicleMovementComponent->RearLeftWheelOffset = FVector(-1.049995f, -1.0f, -1.5f) * CarBox.Y / 4.f;
    VehicleMovementComponent->RearRightWheelOffset = FVector(1.049995f, -1.0f, -1.5f) * CarBox.Y / 4.f;

    /*VehicleMovementComponent->FrontLeftWheelOffset = FVector(CarBox.Y * -0.35f, CarBox.X * -0.3f, CarBox.Z * 0.25f);
    VehicleMovementComponent->FrontRightWheelOffset = FVector(CarBox.Y * -0.35f, CarBox.X * 0.3f, CarBox.Z * 0.25f);
    VehicleMovementComponent->RearLeftWheelOffset = FVector(CarBox.Y * 0.3f, CarBox.X * -0.3f, CarBox.Z * 0.25f);
    VehicleMovementComponent->RearRightWheelOffset = FVector(CarBox.Y * 0.3f, CarBox.X * 0.3f, CarBox.Z * 0.25f);*/
}

USkeletalMeshComponent* AWheeledVehiclePawn::GetMesh() const
{
    return GetComponentByClass<USkeletalMeshComponent>();
}

UVehicleMovementComponent* AWheeledVehiclePawn::GetVehicleMovementComponent() const
{
    return GetComponentByClass<UVehicleMovementComponent>();
}
