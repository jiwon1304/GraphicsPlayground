#include "PhysicsScene.h"
#include "Components/ShapeComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Classes/GameFramework/Actor.h"
#include "PhysicsCore/PhysXIntegration.h"
#include "PhysicsSolver.h"
#include "PhysxSolversModule.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"

void FPhysScene::Init(FPhysicsSolver* InSceneSolver, physx::PxScene* InScene)
{
    if (!InSceneSolver)
    {
        UE_LOG(ELogLevel::Error, "SceneSolver is null!");
        return;
    }

    SceneSolver = InSceneSolver;

    PhysxScene = InScene;
}

void FPhysScene::AddActor(AActor* Actor)
{
    TSet<UActorComponent*> ActorComponents = Actor->GetComponents();

    // StaticMeshComponent
    {
        TSet<UStaticMeshComponent*> StaticMeshComponents;
        for (UActorComponent* Component : ActorComponents)
        {
            if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
            {
                StaticMeshComponents.Add(StaticMeshComponent);
            }
        }

        for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
        {
            UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
            if (!StaticMesh)
            {
                UE_LOG(ELogLevel::Warning, TEXT("StaticMeshComponent '%s' has no StaticMesh assigned."), *StaticMeshComponent->GetName());
                continue;
            }
            
            // 복사해서 붙여줌
            FBodyInstance* BodyInstance = new FBodyInstance(StaticMesh->GetBodySetup()->DefaultInstance);
            
            BodyInstance->OwnerComponent = StaticMeshComponent;
            PxActor* RegisteredActor = SceneSolver->RegisterObject(this, BodyInstance);

            //RegisteredInstances.Add(BodyInstance, RegisteredActor);
        }
    }

    // TODO : SkeletalMeshComponent
}

void FPhysScene::AdvanceAndDispatch_External(float DeltaTime)
{
    SceneSolver->AdvanceOneTimeStep(this, DeltaTime);
}

void FPhysScene::SyncBodies()
{
    SceneSolver->FetchData(this);
}

void FPhysScene::SetGravity(FVector InGravity)
{
    if (PhysxScene)
    {
        PhysxScene->setGravity(PxVec3(InGravity.X, InGravity.Y, InGravity.Z));
        Gravity = InGravity;
    }
    else
    {
        UE_LOG(ELogLevel::Error, "PhysxScene is null!");
    }
}
