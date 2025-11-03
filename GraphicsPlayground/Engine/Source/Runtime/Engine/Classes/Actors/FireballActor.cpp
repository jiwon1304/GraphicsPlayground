#include "FireballActor.h"
#include "Classes/Engine/Asset/FObjLoader.h"

#include "Classes/Components/Light/PointLightComponent.h"

#include "Classes/Components/ProjectileMovementComponent.h"

#include "Classes/Components/SphereComp.h"

AFireballActor::AFireballActor()
{
    FObjManager::CreateStaticMesh("Contents/Sphere.obj");


    SphereComp = AddComponent<USphereComp>("USphereComp_0");
    
    SphereComp->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Sphere.obj"));
  
    PointLightComponent = AddComponent<UPointLightComponent>("UPointLightComponent_0");
    
    PointLightComponent->SetLightColor(FLinearColor::Red);
    
    ProjectileMovementComponent = AddComponent<UProjectileMovementComponent>("UProjectileMovementComponent_0");
    PointLightComponent->AttachToComponent(RootComponent);

    ProjectileMovementComponent->SetGravity(0);
    ProjectileMovementComponent->SetVelocity(FVector(100, 0, 0));
    ProjectileMovementComponent->SetInitialSpeed(100);
    ProjectileMovementComponent->SetMaxSpeed(100);
    ProjectileMovementComponent->SetLifetime(10);
}

void AFireballActor::BeginPlay()
{
}
