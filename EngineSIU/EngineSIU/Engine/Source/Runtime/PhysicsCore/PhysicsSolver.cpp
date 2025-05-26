#include "PhysicsSolver.h"
#include "PhysicsEngine/ShapeElem.h"
#include "PhysicsScene.h"
#include "PhysicsCore/PhysxSolversModule.h"
#include "PhysicsEngine/BodySetup.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/Casts.h"

void FPhysicsSolver::Init()
{

}

void FPhysicsSolver::Release()
{

}


void FPhysicsSolver::InitScene(FPhysScene* InScene) const
{
    if (!InScene)
    {
        UE_LOG(ELogLevel::Error, "InScene is null!");
        return;
    }

    physx::PxScene* NewPxScene = FPhysxSolversModule::GetModule()->CreateScene();
    if (InScene == nullptr || NewPxScene == nullptr)
    {
        UE_LOG(ELogLevel::Error, "Failed to create FPhysScene or PxScene!");
        return;
    }
    InScene->Init(const_cast<FPhysicsSolver*>(this), NewPxScene);
}

// TODO : 일단 FBodyInstance를 사용해서 함
// 이후에 Register를 UstaticMesh, USkeletalMesh, 또는 Actor단위로 받아서 할 것.
PxActor* FPhysicsSolver::RegisterObject(FPhysScene* InScene, const FBodyInstance* NewInstance)
{
    if (!NewInstance)
    {
        UE_LOG(ELogLevel::Error, "NewInstance is null!");
        return nullptr;
    }

    PxScene* Scene = InScene->PhysxScene;
    if (!Scene)
    {
        UE_LOG(ELogLevel::Error, "PxScene is null!");
        return nullptr;
    }

    FMatrix InitialMatrix = NewInstance->OwnerComponent->GetWorldMatrix().GetMatrixWithoutScale();
    FVector InitialPosition = InitialMatrix.GetTranslationVector();
    FQuat InitialRotation = InitialMatrix.ToQuat();
    
    PxVec3 Position(InitialPosition.X, InitialPosition.Y, InitialPosition.Z);
    PxQuat Rotation(InitialRotation.X, InitialRotation.Y, InitialRotation.Z, InitialRotation.W);

    PxTransform InitialTransform(Position, Rotation);

    PxPhysics* Physics = FPhysxSolversModule::GetModule()->Physics;

    PxRigidActor* NewRigidActor = nullptr;

    // Static인지 Dynamic인지 구분
    switch (NewInstance->ObjectType)
    {
    case ECollisionChannel::ECC_WorldStatic:
    {
        PxRigidStatic* NewRigidStatic = Physics->createRigidStatic(InitialTransform);
        if (NewInstance->bSimulatePhysics)
        {
            UE_LOG(ELogLevel::Warning, TEXT("WorldStatic cannot simulate physics!"));
        }
        NewRigidActor = NewRigidStatic; // Static Actor로 설정
        break;
    }
    case ECollisionChannel::ECC_WorldDynamic:
    {
        PxRigidDynamic* NewDynamicActor = Physics->createRigidDynamic(InitialTransform);
        if (NewInstance->bSimulatePhysics)
        {
            NewDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false); // 시뮬레이션 결과를 따르게 설정
        }
        else
        {
            NewDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true); // 시뮬레이션 결과를 무시하게 설정
        }
        NewRigidActor = NewDynamicActor; // Dynamic Actor로 설정
        break;
    }
    default:
    {
        NewRigidActor = nullptr; // default에서도 명시적으로 초기화
        assert(0);
        break;
    }
    }

    // 중력에 영향을 받는지 구분
    switch (NewInstance->bEnableGravity)
    {
    case true:
        NewRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
        break;
    case false:
        NewRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
        break;
    }

    NewRigidActor->setActorFlag(PxActorFlag::eVISUALIZATION, true); // 디버그 렌더링 활성화
    NewRigidActor->setName(*NewInstance->OwnerComponent->GetName()); // 이름 설정

    NewRigidActor->userData = (void*)NewInstance; // 사용자 정의 데이터로 FBodyInstance를 설정

    const FKAggregateGeom& AggGeom = NewInstance->ExternalCollisionProfileBodySetup->AggGeom;
    float Volume = 0.f;

    for (const FKBoxElem& BoxElem : AggGeom.BoxElems)
    {
        PxBoxGeometry BoxGeometry(PxVec3(BoxElem.X, BoxElem.Y, BoxElem.Z)); // 크기
        PxShape* NewShape = Physics->createShape(BoxGeometry, *FPhysxSolversModule::GetModule()->DefaultMaterial); // 
        
        FVector Center = BoxElem.Center;
        FQuat Quat = BoxElem.Rotation.Quaternion();
        NewShape->setLocalPose(PxTransform(PxVec3(Center.X, Center.Y, Center.Z), PxQuat(Quat.X, Quat.Y, Quat.Z, Quat.W)));
        NewRigidActor->attachShape(*NewShape);
        
        Volume = Volume + BoxElem.X * BoxElem.Y * BoxElem.Z;
    }

    for (const FKSphereElem& SphereElem : AggGeom.SphereElems)
    {
        PxSphereGeometry SphereGeometry(SphereElem.Radius);
        PxShape* NewShape = Physics->createShape(SphereGeometry, *FPhysxSolversModule::GetModule()->DefaultMaterial);

        FVector Center = SphereElem.Center;
        NewShape->setLocalPose(PxTransform(PxVec3(Center.X, Center.Y, Center.Z)));
        NewRigidActor->attachShape(*NewShape);

        Volume += 4.0f / 3.0f * PI * FMath::Pow(SphereElem.Radius, 3);
    }

    for (const FKSphylElem& SphylElem : AggGeom.SphylElems)
    {
        PxCapsuleGeometry CapsuleGeometry(SphylElem.Radius, SphylElem.Length);
        PxShape* NewShape = Physics->createShape(CapsuleGeometry, *FPhysxSolversModule::GetModule()->DefaultMaterial);
        FVector Center = SphylElem.Center;
        FQuat Quat = SphylElem.Rotation.Quaternion();
        NewShape->setLocalPose(PxTransform(PxVec3(Center.X, Center.Y, Center.Z), PxQuat(Quat.X, Quat.Y, Quat.Z, Quat.W)));
        NewRigidActor->attachShape(*NewShape);
        Volume += PI * FMath::Pow(SphylElem.Radius, 2) * SphylElem.Length // 중간의 원기둥
            + 4.0f / 3.0f * PI * FMath::Pow(SphylElem.Radius, 3); // 양 끝부분 구
    }

    if (PxRigidDynamic* RigidDynamic = NewRigidActor->is<PxRigidDynamic>())
    {
        // Dynamic Actor의 경우, PhysMaterial이 설정되어 있어야 함
        if (!NewInstance->ExternalCollisionProfileBodySetup->PhysMaterial)
        {
            UE_LOG(ELogLevel::Warning, TEXT("Dynamic Actor must have a PhysMaterial. Setting Density to 1"));
            physx::PxRigidBodyExt::updateMassAndInertia(*RigidDynamic, 1);
        }
        else
        {
            float DensityScaled = NewInstance->MassScale
                * NewInstance->ExternalCollisionProfileBodySetup->PhysMaterial->Density;
            physx::PxRigidBodyExt::updateMassAndInertia(*RigidDynamic, DensityScaled);
        }
    }
    Scene->addActor(*NewRigidActor);

    return NewRigidActor;
}

void FPhysicsSolver::AdvanceOneTimeStep(FPhysScene* InScene, float Dt)
{
    //PxSceneWriteLock scopedWriteLock(*InScene->PhysxScene);
    InScene->PhysxScene->simulate(Dt);
}

void FPhysicsSolver::FetchData(FPhysScene* InScene)
{
    //PxSceneReadLock scopedReadLock(*InScene->PhysxScene);
    InScene->PhysxScene->fetchResults(true);

    PxScene* Scene = InScene->PhysxScene;
    PxU32 TotalActors = Scene->getNbActors(
        PxActorTypeFlag::eRIGID_DYNAMIC
    );
    PxActor** Actors = new PxActor* [TotalActors];

    PxU32 NumReturnActors = Scene->getActors(
        PxActorTypeFlag::eRIGID_DYNAMIC,
        Actors, TotalActors
    );

    for (PxU32 i = 0; i < NumReturnActors; ++i)
    {
        PxRigidDynamic* DynamicActor = Actors[i]->is<PxRigidDynamic>();
        PxTransform Transform = DynamicActor->getGlobalPose();

        FBodyInstance* BodyInstance = static_cast<FBodyInstance*>(DynamicActor->userData);

        if (BodyInstance->OwnerComponent)
        {
            if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(BodyInstance->OwnerComponent))
            {
                BodyInstance->OwnerComponent->SetWorldTransform(
                    FTransform(
                        FQuat(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w),
                        FVector(Transform.p.x, Transform.p.y, Transform.p.z),
                        FVector(BodyInstance->Scale3D.X, BodyInstance->Scale3D.Y, BodyInstance->Scale3D.Z)
                    )
                );
            }
            else if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(BodyInstance->OwnerComponent))
            {
                // !TODO : Bone을 찾아서 해당 Bone의 위치 변경. 또는 Bonematrix 변경
                USkeletalMesh* SkeletalMesh = SkeletalMeshComp->GetSkeletalMeshAsset();
                if (!SkeletalMesh)
                {
                    UE_LOG(ELogLevel::Warning, TEXT("SkeletalMeshComponent '%s' has no SkeletalMesh assigned."), *SkeletalMeshComp->GetName());
                    continue;
                }
                int16 BoneIndex = BodyInstance->InstanceBoneIndex;
                int16 ParentIndex = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().GetRawRefBoneInfo()[BoneIndex].ParentIndex;
                FTransform ParentComponentSpaceTransform = ParentIndex != INDEX_NONE ? SkeletalMeshComp->GetBoneComponentSpaceTransform(ParentIndex) : FTransform::Identity;

                FTransform ComponentSpaceTransform = SkeletalMeshComp->GetComponentTransform();

                FTransform SimulatedWorldTransform = FTransform(
                    FQuat(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w),
                    FVector(Transform.p.x, Transform.p.y, Transform.p.z),
                    FVector(BodyInstance->Scale3D.X, BodyInstance->Scale3D.Y, BodyInstance->Scale3D.Z)
                );

                FTransform NewBoneTransform = SimulatedWorldTransform * ComponentSpaceTransform.Inverse() *
                    ParentComponentSpaceTransform.Inverse(); // 컴포넌트 공간 트랜스폼을 적용하여 본의 위치를 계산

                SkeletalMeshComp->GetBonePoseContext().Pose[BoneIndex] = NewBoneTransform; // 본 위치 갱신
            }
            
        }
    }
}

PxGeometryType::Enum FPhysicsSolver::GetPxType(const FKShapeElem* InShape)
{
    if (InShape->StaticStruct()->IsChildOf(FKBoxElem::StaticStruct()))
    {
        return PxGeometryType::eBOX;
    }
    else
    {
        assert(0);
    }
}
