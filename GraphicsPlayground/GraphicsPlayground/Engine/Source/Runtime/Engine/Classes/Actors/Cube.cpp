#include "Cube.h"

#include "Classes/Components/BoxComponent.h"
#include "Classes/Components/CapsuleComponent.h"
#include "Classes/Components/SphereComponent.h"
#include "Classes/Components/StaticMeshComponent.h"

#include "Classes/Engine/FObjLoader.h"

#include "Classes/GameFramework/Actor.h"

ACube::ACube()
{
    StaticMeshComponent->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Reference/Reference.obj"));

    
}

void ACube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //SetActorRotation(GetActorRotation() + FRotator(0, 0, 1));

}
