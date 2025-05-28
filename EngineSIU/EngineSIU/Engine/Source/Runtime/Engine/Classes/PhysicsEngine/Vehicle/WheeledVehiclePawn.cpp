#include "WheeledVehiclePawn.h"
#include "Engine/Classes/Engine/AssetManager.h"

AWheeledVehiclePawn::AWheeledVehiclePawn()
{
    MeshComponent = AddComponent<USkeletalMeshComponent>();
    SetRootComponent(MeshComponent);

    MeshComponent->SetSkeletalMeshAsset(UAssetManager::Get().GetSkeletalMesh("Contents/Asset/Sharkry_Dancing"));
    MeshComponent->SetupAttachment(nullptr);

    VehicleMovementComponent = AddComponent<UVehicleMovementComponent>();
}
