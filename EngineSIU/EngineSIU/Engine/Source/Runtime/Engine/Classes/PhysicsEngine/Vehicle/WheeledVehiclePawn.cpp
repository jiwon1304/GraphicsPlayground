#include "WheeledVehiclePawn.h"
#include "Engine/Classes/Engine/AssetManager.h"
#include "Engine/FObjLoader.h"

AWheeledVehiclePawn::AWheeledVehiclePawn()
{
    
}

void AWheeledVehiclePawn::PostSpawnInitialize()
{
    // 아래 2개 중 하나  택1 하나는 주석처리

    // SkeltalMesh 쓸거면 아래껄로
    USkeletalMeshComponent* MeshComponent = AddComponent<USkeletalMeshComponent>();
    SetRootComponent(MeshComponent);
    MeshComponent->SetSkeletalMeshAsset(UAssetManager::Get().GetSkeletalMesh("Contents/Asset/SportsVehicle"));

    // StaticMesh 쓸거면 아래껄로
    /*UStaticMeshComponent* MeshComponent = AddComponent<UStaticMeshComponent>();
    SetRootComponent(MeshComponent);
    MeshComponent->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Asset/Car/CarBody.obj"));*/


    MeshComponent->SetupAttachment(nullptr);
    MeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

    UVehicleMovementComponent* VehicleMovementComponent = AddComponent<UVehicleMovementComponent>();

    FVector CarBox = MeshComponent->AABB.MaxLocation - MeshComponent->AABB.MinLocation;
    CarBox = CarBox * 0.1f;
    
    // 모델 마다 x축 혹은 y 축이 앞임
    float CarLength = CarBox.Y;
    float CarWidth = CarBox.X;
    float CarHeight = CarBox.Z;


    VehicleMovementComponent->ChassisBoxExtents = FVector(CarWidth, CarHeight, CarLength);

    VehicleMovementComponent->WheelRadius = CarLength / 5.5f;
    VehicleMovementComponent->WheelWidth = CarWidth / 4.5f;

    VehicleMovementComponent->FrontLeftWheelOffset = FVector(-1.049995f, -1.0f, 1.5f) * CarLength / 4.f;
    VehicleMovementComponent->FrontRightWheelOffset = FVector(1.049995f, -1.0f, 1.5f) * CarLength / 4.f;
    VehicleMovementComponent->RearLeftWheelOffset = FVector(-1.049995f, -1.0f, -1.5f) * CarLength / 4.f;
    VehicleMovementComponent->RearRightWheelOffset = FVector(1.049995f, -1.0f, -1.5f) * CarLength / 4.f;

    /*VehicleMovementComponent->FrontLeftWheelOffset = FVector(CarBox.Y * -0.35f, CarBox.X * -0.3f, CarBox.Z * 0.25f);
    VehicleMovementComponent->FrontRightWheelOffset = FVector(CarBox.Y * -0.35f, CarBox.X * 0.3f, CarBox.Z * 0.25f);
    VehicleMovementComponent->RearLeftWheelOffset = FVector(CarBox.Y * 0.3f, CarBox.X * -0.3f, CarBox.Z * 0.25f);
    VehicleMovementComponent->RearRightWheelOffset = FVector(CarBox.Y * 0.3f, CarBox.X * 0.3f, CarBox.Z * 0.25f);*/
}

USkeletalMeshComponent* AWheeledVehiclePawn::GetSkeltalMesh() const
{
    return GetComponentByClass<USkeletalMeshComponent>();
}

UStaticMeshComponent* AWheeledVehiclePawn::GetStaticMesh() const
{
    return GetComponentByClass<UStaticMeshComponent>();
}

UVehicleMovementComponent* AWheeledVehiclePawn::GetVehicleMovementComponent() const
{
    return GetComponentByClass<UVehicleMovementComponent>();
}
