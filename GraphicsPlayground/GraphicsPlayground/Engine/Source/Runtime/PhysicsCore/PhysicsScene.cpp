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
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "Engine/Classes/PhysicsEngine/Vehicle/WheeledVehiclePawn.h"

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

void FPhysScene::Release()
{
    if (PhysxScene)
    {
        PhysxScene->release();
    }

    if (SceneSolver)
    {
        delete SceneSolver;
    }
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
            // !TODO : 오버라이드하는 속성들이 있으면 여기서 적용해서 인스턴스 생성

            FBodyInstance* BodyInstance = new FBodyInstance(StaticMesh->GetBodySetup()->DefaultInstance);
            
            BodyInstance->OwnerComponent = StaticMeshComponent;
            PxActor* RegisteredActor = SceneSolver->RegisterObject(this, BodyInstance, StaticMeshComponent->GetWorldMatrix().GetMatrixWithoutScale());

            //StaticMeshComponent->BodyInstance = BodyInstance;
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

            TMap<FName, PxActor*> RegisteredActors;
            for (const auto& BodySetup : PhysicsAsset->BodySetup)
            {
                // 복사해서 붙여줌
                FBodyInstance* BodyInstance = new FBodyInstance(BodySetup->DefaultInstance);
                BodyInstance->OwnerComponent = SkeletalMeshComponent;
                // !TODO : geometry들에 대해서 Bone 기준 좌표계로 넘긴다
                FName BoneName = BodySetup->BoneName;
                int32 BoneIndex = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().FindRawBoneIndex(BoneName);
                if (BoneIndex == INDEX_NONE)
                {
                    UE_LOG(ELogLevel::Warning, TEXT("BodySetup '%s' has no valid BoneName '%s' in SkeletalMesh '%s'."), *BodySetup->GetName(), *BoneName.ToString(), *SkeletalMesh->GetName());
                    delete BodyInstance; // 메모리 해제
                    continue;
                }

                FMatrix InitialMatrix = /**/ SkeletalMeshComponent->GetBoneComponentSpaceTransform(BoneIndex).ToMatrixNoScale() * SkeletalMeshComponent->GetWorldMatrix();

                PxActor* RegisteredActor = SceneSolver->RegisterObject(this, BodyInstance, InitialMatrix);

                if (RegisteredActor)
                {
                    // 본 이름을 키로 해서 캐시 -> Constraint에서 찾아서 사용
                    RegisteredActors.Add(BodySetup->BoneName, RegisteredActor);
                    SkeletalMeshComponent->Bodies.Add(BodyInstance);
                }
                else
                {
                    UE_LOG(ELogLevel::Error, TEXT("Failed to register BodyInstance for SkeletalMeshComponent '%s'."), *SkeletalMeshComponent->GetName());
                    delete BodyInstance; // 메모리 해제
                }
            }

            // constraints
            for (const auto& ConstraintSetup : PhysicsAsset->ConstraintSetup)
            {
                FConstraintInstance* ConstraintInstance = new FConstraintInstance(ConstraintSetup->DefaultInstance);
                FName ChildBone = ConstraintInstance->ConstraintBone1;
                PxActor* ChildActor = nullptr;
                if (RegisteredActors.Find(ChildBone))
                {
                    ChildActor = RegisteredActors[ChildBone];
                }
                else
                {
                    UE_LOG(ELogLevel::Warning, TEXT("Constraint '%s' has no registered actor for Bone1 '%s'."), *ConstraintSetup->GetName(), *ChildBone.ToString());
                    delete ConstraintInstance; // 메모리 해제
                    continue;
                }
                FName ParentBone = ConstraintInstance->ConstraintBone2;
                PxActor* ParentActor = nullptr;
                if (RegisteredActors.Find(ParentBone))
                {
                    ParentActor = RegisteredActors[ParentBone];
                }
                else
                {
                    UE_LOG(ELogLevel::Warning, TEXT("Constraint '%s' has no registered actor for Bone2 '%s'."), *ConstraintSetup->GetName(), *ParentBone.ToString());
                    delete ConstraintInstance; // 메모리 해제
                    continue;
                }
                PxJoint* NewJoint = SceneSolver->CreateJoint(this, ChildActor, ParentActor, ConstraintInstance);

                SkeletalMeshComponent->Constraints.Add(ConstraintInstance);
            }
        }
    }
}

void FPhysScene::AddVehicle(AWheeledVehiclePawn* Vehicle)
{
    USceneComponent* MeshComponent = Vehicle->GetRootComponent();

    USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComponent);
    UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent);


    // 현재는 FBodyInstance는 OwnerComponent 연결 부분으로만 사용하는데
    // 이후 수정 필요할듯
    FBodyInstance* VehicleMainBodyInstance = new FBodyInstance();
    FMatrix InitialMatrix;

    VehicleMainBodyInstance->bCar = true;

    if (SkeletalMeshComponent != nullptr) 
    {
        VehicleMainBodyInstance->OwnerComponent = SkeletalMeshComponent;
        InitialMatrix = SkeletalMeshComponent->GetWorldMatrix();
        VehicleMainBodyInstance->Scale3D = SkeletalMeshComponent->GetRelativeScale3D();
    }
    else if (StaticMeshComponent != nullptr) 
    {
        VehicleMainBodyInstance->OwnerComponent = StaticMeshComponent;
        InitialMatrix = StaticMeshComponent->GetWorldMatrix();
        VehicleMainBodyInstance->Scale3D = StaticMeshComponent->GetRelativeScale3D();
    }

    SceneSolver->RegisterObject(this, VehicleMainBodyInstance, Vehicle->GetVehicleMovementComponent(), InitialMatrix);
}

void FPhysScene::AdvanceAndDispatch_External(float DeltaTime)
{
    SceneSolver->AdvanceOneTimeStep(this, DeltaTime);
}

void FPhysScene::SyncBodies()
{
    SceneSolver->FetchData(this);
}

void FPhysScene::SetGeometryToWorld(UBodySetup* BodySetup)
{
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
