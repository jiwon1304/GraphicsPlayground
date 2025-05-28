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
#include "PhysicsEngine/ConstraintInstance.h"
#include "Developer/PhysicsUtilities/PhysicsAssetUtils.h"
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
physx::PxActor* FPhysicsSolver::RegisterObject(FPhysScene* InScene, const FBodyInstance* NewInstance, const FMatrix& InitialMatrix)
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
        PxCapsuleGeometry CapsuleGeometry(SphylElem.Radius, SphylElem.Length / 2.f);
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

physx::PxJoint* FPhysicsSolver::CreateJoint(FPhysScene* InScene, PxActor* Child, PxActor* Parent, const FConstraintInstance* NewInstance)
{
    PxRigidDynamic* ParentDynamic = Parent->is<PxRigidDynamic>();
    PxRigidDynamic* ChildDynamic = Child->is<PxRigidDynamic>();
    if (!ParentDynamic || !ChildDynamic)
    {
        UE_LOG(ELogLevel::Error, TEXT("Parent or Child is not a dynamic actor!"));
        return nullptr;
    }

   // PhysX 액터의 글로벌 포즈를 가져옵니다 (이미 PhysX 좌표계 기준).
    PxTransform parentWorld = ParentDynamic->getGlobalPose();
    PxTransform childWorld = ChildDynamic->getGlobalPose();

    PxQuat q_AxisCorrection = PxQuat(PxMat33(
        PxVec3(0.0f, 0.0f, 1.0f),
        PxVec3(1.0f, 0.0f, 0.0f), 
        PxVec3(0.0f, 1.0f, 0.0f)  
    ));
    q_AxisCorrection.normalize(); // 정규화

    // 자식 액터에 대한 조인트의 로컬 프레임입니다.
    // 위치는 자식 액터의 원점, 방향은 위에서 정의한 축 보정 회전을 적용합니다.
    PxTransform localFrameChild(PxVec3(0.0f), q_AxisCorrection);

    PxTransform childJointFrameInWorld = childWorld * localFrameChild;
    PxTransform localFrameParent = parentWorld.getInverse() * childJointFrameInWorld;

    PxD6Joint* Joint = PxD6JointCreate(*FPhysxSolversModule::GetModule()->Physics, ParentDynamic, localFrameParent, ChildDynamic, localFrameChild);

    // 3. 프로파일 속성 적용
    const FConstraintProfileProperties& Profile = NewInstance->ProfileInstance;
    const FLinearConstraint& LinearLimitProps = Profile.LinearLimit;
    const FConeConstraint& ConeLimitProps = Profile.ConeLimit;
    const FTwistConstraint& TwistLimitProps = Profile.TwistLimit;

    //Joint->setMotion(physx::PxD6Axis::eX, FPhysicsAssetUtils::MapLinearMotionToPx(LinearLimitProps.XMotion));
    //Joint->setMotion(physx::PxD6Axis::eY, FPhysicsAssetUtils::MapLinearMotionToPx(LinearLimitProps.YMotion));
    //Joint->setMotion(physx::PxD6Axis::eZ, FPhysicsAssetUtils::MapLinearMotionToPx(LinearLimitProps.ZMotion));

    // 선형 제한
    if (LinearLimitProps.XMotion != ELinearConstraintMotion::LCM_Free ||
        LinearLimitProps.YMotion != ELinearConstraintMotion::LCM_Free ||
        LinearLimitProps.ZMotion != ELinearConstraintMotion::LCM_Free)
    {
        physx::PxReal extentVal;
        // 모든 선형 축이 Locked 상태인지 확인
        bool bAllLinearLocked = LinearLimitProps.XMotion == ELinearConstraintMotion::LCM_Locked &&
            LinearLimitProps.YMotion == ELinearConstraintMotion::LCM_Locked &&
            LinearLimitProps.ZMotion == ELinearConstraintMotion::LCM_Locked;

        if (bAllLinearLocked) 
        {
            extentVal = 0.0f; // 모든 축이 잠겼으면 제한 범위 0
        }
        else 
        {
            // 하나라도 Limited이고 나머지가 Locked/Free인 경우, LinearLimitProps.Limit 사용
            // setMotion이 각 축의 Locked/Free 상태를 이미 처리함
            extentVal = LinearLimitProps.Limit;
            if (extentVal < 0) extentVal = FMath::Abs(extentVal);
        }

        physx::PxReal stiffness = 0.0f;
        physx::PxReal damping = 0.0f;

        if (LinearLimitProps.bSoftConstraint) 
        {
            stiffness = LinearLimitProps.Stiffness;
            damping = LinearLimitProps.Damping;
        }

        physx::PxSpring spring(stiffness, damping);
        physx::PxJointLinearLimit limitParams(extentVal, spring);
        //Joint->setLinearLimit(limitParams);
    }

    // 각도 제한
    Joint->setMotion(physx::PxD6Axis::eSWING1, FPhysicsAssetUtils::MapAngularMotionToPx(ConeLimitProps.Swing1Motion));
    Joint->setMotion(physx::PxD6Axis::eSWING2, FPhysicsAssetUtils::MapAngularMotionToPx(ConeLimitProps.Swing2Motion));

    if (ConeLimitProps.Swing1Motion != EAngularConstraintMotion::ACM_Free ||
        ConeLimitProps.Swing2Motion != EAngularConstraintMotion::ACM_Free)
    {
        physx::PxReal coneStiffness = 0.0f;
        physx::PxReal coneDamping = 0.0f;

        if (ConeLimitProps.bSoftConstraint) 
        {
            coneStiffness = ConeLimitProps.Stiffness;
            coneDamping = ConeLimitProps.Damping;
        }
        physx::PxSpring spring(coneStiffness, coneDamping);

        // PxJointLimitCone 생성자는 (Swing2관련 각도, Swing1관련 각도, PxSpring) 순서로 가정
        physx::PxReal swing2Angle, swing1Angle;

        if (ConeLimitProps.Swing2Motion == EAngularConstraintMotion::ACM_Locked) 
        {
            swing2Angle = 0.0f;
        }
        else 
        { // ACM_Limited 또는 ACM_Free (이 경우 LimitDegrees 값 사용)
            swing2Angle = FMath::DegreesToRadians(ConeLimitProps.Swing2LimitDegrees);
        }

        if (ConeLimitProps.Swing1Motion == EAngularConstraintMotion::ACM_Locked) 
        {
            swing1Angle = 0.0f;
        }
        else 
        { // ACM_Limited 또는 ACM_Free
            swing1Angle = FMath::DegreesToRadians(ConeLimitProps.Swing1LimitDegrees);
        }

        // PxJointLimitCone은 양의 각도를 기대하므로 절대값 처리
        swing1Angle = FMath::Abs(swing1Angle);
        swing2Angle = FMath::Abs(swing2Angle);

        physx::PxJointLimitCone coneLimitParams
        (
            swing2Angle,
            swing1Angle,
            spring
        );
        Joint->setSwingLimit(coneLimitParams);
    }

    // 트위스트 제한
    Joint->setMotion(physx::PxD6Axis::eTWIST, FPhysicsAssetUtils::MapAngularMotionToPx(TwistLimitProps.TwistMotion));

    if (TwistLimitProps.TwistMotion != EAngularConstraintMotion::ACM_Free)
    {
        physx::PxReal twistStiffness = 0.0f;
        physx::PxReal twistDamping = 0.0f;
        if (TwistLimitProps.bSoftConstraint)
        {
            twistStiffness = TwistLimitProps.Stiffness;
            twistDamping = TwistLimitProps.Damping;
        }
        physx::PxSpring spring(twistStiffness, twistDamping);

        physx::PxReal halfAngleRad;
        if (TwistLimitProps.TwistMotion == EAngularConstraintMotion::ACM_Locked) 
        {
            halfAngleRad = 0.0f;
        }
        else 
        { // ACM_Limited
            halfAngleRad = FMath::DegreesToRadians(TwistLimitProps.TwistLimitDegrees);
            halfAngleRad = FMath::Abs(halfAngleRad); // 대칭 제한을 위해 양수 값으로
        }

        physx::PxJointAngularLimitPair twistLimitParams
        (
            -halfAngleRad, // 최소 각도
            halfAngleRad,  // 최대 각도
            spring          // 스프링 설정
        );

        Joint->setTwistLimit(twistLimitParams);
    }


    // !TODO : Bodysetup단에서 bCollisionEnabled를 가져와서 세팅
    Joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, !Profile.bDisableCollision); 
#if _DEBUG
    Joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
#endif
    return Joint;
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
                        BodyInstance->OwnerComponent->GetComponentScale3D()
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

                FTransform CachedBoneWorldTransform = SkeletalMeshComp->GetComponentTransform() * SkeletalMeshComp->GetBoneComponentSpaceTransform(BoneIndex);
                FVector OriginScale = CachedBoneWorldTransform.Scale3D;
                FTransform SimulatedWorldTransform = FTransform(
                    FQuat(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w),
                    FVector(Transform.p.x, Transform.p.y, Transform.p.z),
                    OriginScale
                );

                FTransform NewBoneTransform = (ParentComponentSpaceTransform.Inverse() * ComponentSpaceTransform.Inverse())
                    * SimulatedWorldTransform; // 컴포넌트 공간 트랜스폼을 적용하여 본의 위치를 계산

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
