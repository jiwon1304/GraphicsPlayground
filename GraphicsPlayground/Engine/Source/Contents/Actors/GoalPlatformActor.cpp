
#include "GoalPlatformActor.h"

#include "Classes/Engine/Asset/FObjLoader.h"

AGoalPlatformActor::AGoalPlatformActor()
{
    BoxComponent = AddComponent<UBoxComponent>(FName("BoxComponent_0"));
    RootComponent = BoxComponent;

    MeshComponent = AddComponent<UStaticMeshComponent>(FName("MeshComponent_0"));
    MeshComponent->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/FryBasket/FryBasket.obj"));
    MeshComponent->SetupAttachment(BoxComponent);
}
