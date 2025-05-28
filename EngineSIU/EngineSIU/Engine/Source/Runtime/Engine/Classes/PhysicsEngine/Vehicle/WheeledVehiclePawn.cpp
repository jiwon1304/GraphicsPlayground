#include "WheeledVehiclePawn.h"
#include "Engine/Classes/Engine/AssetManager.h"

AWheeledVehiclePawn::AWheeledVehiclePawn()
{
    USkeletalMeshComponent* MeshComponent = AddComponent<USkeletalMeshComponent>();
    SetRootComponent(MeshComponent);

    MeshComponent->SetSkeletalMeshAsset(UAssetManager::Get().GetSkeletalMesh("Contents/Asset/Sharkry_Dancing"));
    MeshComponent->SetupAttachment(nullptr);

    AddComponent<UVehicleMovementComponent>();
}

USkeletalMeshComponent* AWheeledVehiclePawn::GetMesh() const
{
    return GetComponentByClass<USkeletalMeshComponent>();
}

UVehicleMovementComponent* AWheeledVehiclePawn::GetVehicleMovementComponent() const
{
    return GetComponentByClass<UVehicleMovementComponent>();
}
