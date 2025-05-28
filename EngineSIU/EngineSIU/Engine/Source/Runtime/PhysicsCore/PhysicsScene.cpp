#include "PhysicsScene.h"
#include "Components/ShapeComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Classes/GameFramework/Actor.h"
#include "PhysicsCore/PhysXIntegration.h"
#include "PhysicsSolver.h"
#include "PhysxSolversModule.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Engine/Classes/PhysicsEngine/Vehicle/WheeledVehiclePawn.h"
#include "Engine/Classes/PhysicsEngine/PhysicsAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Classes/Engine/SkeletalMesh.h"

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
    // AWheeledVehiclePawn인 경우에 처리를 다르게 해줘야 하므로 함수 분리
    if (AWheeledVehiclePawn* WheeledVehiclePawn = Cast<AWheeledVehiclePawn>(Actor)) 
    {
        AddVehicle(WheeledVehiclePawn);
        return;
    }

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

void FPhysScene::AddVehicle(AWheeledVehiclePawn* Vehicle)
{
    USkeletalMeshComponent* SkeletalMeshComponent = Vehicle->GetMesh();
    if (!SkeletalMeshComponent) 
    {
        UE_LOG(ELogLevel::Warning, TEXT("AWheeledVehiclePawn '%s' has no SkeletalMeshComponent assigned."), *Vehicle->GetName());
        return;
    }

    USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();

    // 현재는 FBodyInstance는 OwnerComponent 연결 부분으로만 사용하는데
    // 이후 수정 필요할듯
    FBodyInstance* VehicleMainBodyInstance = new FBodyInstance();
    VehicleMainBodyInstance->OwnerComponent = SkeletalMeshComponent;

    SceneSolver->RegisterObject(this, VehicleMainBodyInstance, Vehicle->GetVehicleMovementComponent());
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
