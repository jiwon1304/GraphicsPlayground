#include "PhysicsScene.h"
#include "Components/ShapeComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Classes/GameFramework/Actor.h"
#include "PhysicsCore/PhysXIntegration.h"
#include "PhysicsSolver.h"
#include "PhysxSolversModule.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"

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

    // SkeletalMeshComponent
    {
        TSet<USkeletalMeshComponent*> SkeletalMeshComponents;
        for (UActorComponent* Component : ActorComponents)
        {
            if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
            {
                SkeletalMeshComponents.Add(SkeletalMeshComponent);
            }
        }
        for (USkeletalMeshComponent* SkeletalMeshComponent : SkeletalMeshComponents)
        {
            USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
            if (!SkeletalMesh)
            {
                UE_LOG(ELogLevel::Warning, TEXT("SkeletalMeshComponent '%s' has no SkeletalMesh assigned."), *SkeletalMeshComponent->GetName());
                continue;
            }
            // 래그돌 정보 생성
            UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
            if (!PhysicsAsset)
            {
                UE_LOG(ELogLevel::Warning, TEXT("SkeletalMesh '%s' has no PhysicsAsset assigned."), *SkeletalMesh->GetName());
                continue;
            }

        }
    }
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
