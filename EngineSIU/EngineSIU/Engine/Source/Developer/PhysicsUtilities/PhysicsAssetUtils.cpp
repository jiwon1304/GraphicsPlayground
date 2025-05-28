#include "PhysicsAssetUtils.h"

#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UObject/ObjectFactory.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicalMaterials/Defines.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"

namespace FPhysicsAssetUtils
{
    bool HasSufficientSkinnedVertices(USkeletalMesh* SkelMesh, int32 BoneIndex, float MintotalWeightThreshold)
    {
        // 해당 본에 할당된 총 버텍스 웨이트의 합을 Threshold와 비교
        return true;
    }
    bool IsboneLargeEnough(USkeletalMesh* SkelMesh, int32 BoneIndex, float MinBoneSizeThreshold)
    {
        // 본의 Bounding Box 크기를 계산하고, MinBoneSizeThreshold와 비교
        return true;
    }
    bool CreateFromSkeletalMesh(UPhysicsAsset* PhysicsAsset, USkeletalMesh* SkelMesh)
    {
        assert(PhysicsAsset);
        PhysicsAsset->PreviewSkeletalMesh = SkelMesh;
        SkelMesh->SetPhysicsAsset(PhysicsAsset);

        assert(SkelMesh);
        assert(SkelMesh->GetRenderData());

        // 1. Bone들을 순회하면서 BodySetup 생성 -> 이거 너무 많이생기는데
        if (!SkelMesh->GetRefSkeleton())
            return false; // Skeleton이 없으면 실패

        // 일단 루트에다가만 한 개 만들기
        const FReferenceSkeleton* ReferenceSkeleton = SkelMesh->GetRefSkeleton();
        const TArray<FMeshBoneInfo>& RawBoneInfo = ReferenceSkeleton->GetRawRefBoneInfo();
        const TArray<FTransform>& RefBonePoses = SkelMesh->GetRefSkeleton()->GetRawRefBonePose();
        for (int i = 0; i < RawBoneInfo.Num(); i++)
        {
            const FMeshBoneInfo& Bone = RawBoneInfo[i];
            // 일단 모든 본에 다 만들어보기
            UBodySetup* BodySetup = FObjectFactory::ConstructObject<UBodySetup>(PhysicsAsset, FName(*FString::Printf(TEXT("BodySetup_%s"), *Bone.Name.ToString())));
            BodySetup->BoneName = Bone.Name;

            BodySetup->CollisionResponse = EBodyCollisionResponse::Type::BodyCollision_Enabled;
            BodySetup->DefaultInstance.ExternalCollisionProfileBodySetup = BodySetup;
            BodySetup->DefaultInstance.InstanceBoneIndex = i;
            BodySetup->DefaultInstance.InstanceBodyIndex = i;
            BodySetup->DefaultInstance.ObjectType = ECollisionChannel::ECC_WorldDynamic;

            FPhysicsMaterial* PhysicsMaterial = new FPhysicsMaterial();
            PhysicsMaterial->Density = 1000.f; // 임시 밀도 값

            // !TODO : 디폴트 머티리얼을 하나 만들어두고 그걸 사용하도록 해야 함
            UPhysicalMaterial* PhysMaterial = FObjectFactory::ConstructObject<UPhysicalMaterial>(nullptr);
            PhysMaterial->Material = PhysicsMaterial;


            FKSphylElem SphylElem;
            bool bIsValidSphyl = true;
            int32 ChildBoneIndex = -1;
            for (int32 j = 0; j < RawBoneInfo.Num(); ++j)
            {
                if (RawBoneInfo[j].ParentIndex == i)
                {
                    ChildBoneIndex = j;
                    break; // 첫 번째 자식 본만 사용
                }
            }

            if (ChildBoneIndex != -1)
            {
                FVector ChildLocalOffset = RefBonePoses[ChildBoneIndex].GetTranslation();
                float DistanceToChild = ChildLocalOffset.Size();

                if (DistanceToChild > KINDA_SMALL_NUMBER)
                {
                    //!TODO : 부모 - 자식을 기준으로 Bone의 길이와 Center 설정
                    //SphylElem.Length = 1.f;
                    //SphylElem.Center = FVector::ZeroVector; // 캡슐의 중심은 부모 본의 위치로 설정
                    //FVector BoneDirection = ChildLocalOffset.GetSafeNormal();
                    //SphylElem.Rotation = FQuat::FindBetween(FVector::XAxisVector, BoneDirection).Rotator();

                    FVector BoneDirection = ChildLocalOffset.GetSafeNormal();
                    SphylElem.Length = FMath::Max(0.1f, ChildLocalOffset.Size() * 0.9f); // 캡슐 길이는 전체 거리의 90%
                    SphylElem.Center = ChildLocalOffset * 0.5f; // 부모에서 자식 방향의 중간 지점
                    SphylElem.Rotation = FQuat::FindBetween(FVector::XAxisVector, BoneDirection).Rotator();
                }
                else
                {
                    bIsValidSphyl = false;
                }
            }
            else
            {
                // 리프노드인데, 그냥 만들지말자
                bIsValidSphyl = false;
            }

            SphylElem.Name = Bone.Name.ToString() + "_capsule";

            BodySetup->AggGeom.SphylElems.Add(SphylElem);
            BodySetup->PhysMaterial = PhysMaterial;

            if (bIsValidSphyl)
                PhysicsAsset->BodySetup.Add(BodySetup);
            PhysicsAsset->UpdateBodySetupIndexMap();
            PhysicsAsset->BoneNameToIndexMap.Add(Bone.Name, PhysicsAsset->BodySetup.Num() - 1);
        }

        // !TODO : 어느정도까지 만들어줄지 생각해야 함
        // 2. ConstraintSetup 생성
        for (int32 ChildBoneIdx = 0; ChildBoneIdx < RawBoneInfo.Num(); ChildBoneIdx++)
        {
            const FMeshBoneInfo& ChildBoneInfo = RawBoneInfo[ChildBoneIdx];
            int32 ParentBoneIdx = ChildBoneInfo.ParentIndex;

            if (ParentBoneIdx != INDEX_NONE && ParentBoneIdx < RawBoneInfo.Num())
            {
                const FName ChildBoneName = ChildBoneInfo.Name;
                const FName ParentBoneName = RawBoneInfo[ParentBoneIdx].Name;

                if (PhysicsAsset->BoneNameToIndexMap.Contains(ChildBoneName) &&
                    PhysicsAsset->BoneNameToIndexMap.Contains(ParentBoneName))
                {
                    //UPhysicsConstraintTemplate 를 만들어서 집어넣어야 함. 부모에
                    UPhysicsConstraintTemplate* ConstraintSetup = FObjectFactory::ConstructObject<UPhysicsConstraintTemplate>(PhysicsAsset, FName(*FString::Printf(TEXT("Constraint_%s_%s"), *ParentBoneName.ToString(), *ChildBoneName.ToString())));

                    FConstraintInstance& JointInstance = ConstraintSetup->DefaultInstance;

                    JointInstance.JointName = ParentBoneName;
                    JointInstance.ConstraintBone1 = ChildBoneName;
                    JointInstance.ConstraintBone2 = ParentBoneName;

                    int32 NewConstraintIndex = PhysicsAsset->ConstraintSetup.Num() - 1;
                    JointInstance.ConstraintIndex = NewConstraintIndex;

                    // !TODO : 적절한 기본 수치 지정
                    // RefBonePoses는 컴포넌트 공간(CS) 트랜스폼입니다.
                    const FTransform& ParentBoneTransformCS = RefBonePoses[ParentBoneIdx];
                    const FTransform& ChildBoneTransformCS = RefBonePoses[ChildBoneIdx];

                    FVector ChildToParentDir_CS = (ParentBoneTransformCS.GetTranslation() - ChildBoneTransformCS.GetTranslation()).GetSafeNormal();
                    if (ChildToParentDir_CS.IsNearlyZero())
                    {
                        ChildToParentDir_CS = FVector(1.0f, 0.0f, 0.0f); // fallback
                    }

                    // 자식 본 기준 프라이머리/세컨더리 축 설정
                    JointInstance.PriAxis2 = ChildToParentDir_CS;
                    JointInstance.SecAxis2 = FVector::UpVector; // 자식 로컬에서 Y축 대용 (Z-up이므로)

                    if (FMath::Abs(FVector::DotProduct(JointInstance.PriAxis2, JointInstance.SecAxis2)) > 0.99f)
                    {
                        JointInstance.SecAxis2 = FVector::RightVector; // PriAxis2와 너무 평행하면 다른 축 사용
                    }

                    // 조인트의 월드 트랜스폼 (자식 본의 트랜스폼과 동일, Pos2/Axis2가 자식 로컬에서 Identity이므로)
                    FTransform JointWorldCS = ChildBoneTransformCS;

                    // 부모 본 로컬 공간에서의 조인트 트랜스폼
                    FTransform JointInParentLocalCS = JointWorldCS.GetRelativeTransform(ParentBoneTransformCS);
                    // 부모 본 기준 프라이머리/세컨더리 축 설정
                    FQuat JointRotationInParentCS = JointInParentLocalCS.GetRotation();
                    JointInstance.PriAxis1 = JointRotationInParentCS.RotateVector(JointInstance.PriAxis2);
                    JointInstance.SecAxis1 = JointRotationInParentCS.RotateVector(JointInstance.SecAxis2);

                    FConstraintProfileProperties& Profile = JointInstance.ProfileInstance; // 참조로 가져옴

                    Profile.LinearLimit.XMotion = ELinearConstraintMotion::LCM_Locked;
                    Profile.LinearLimit.YMotion = ELinearConstraintMotion::LCM_Locked;
                    Profile.LinearLimit.ZMotion = ELinearConstraintMotion::LCM_Locked;
                    Profile.LinearLimit.Limit = 0.0f; // Locked 상태에서는 Limit 값의 의미가 적지만, 기본값 설정
                    Profile.LinearLimit.bSoftConstraint = true;
                    Profile.LinearLimit.Stiffness = 50.f;
                    Profile.LinearLimit.Damping = 5.f;
                    Profile.LinearLimit.Restitution = 0.0f; // PhysX 조인트 생성자에 따라 실제 적용 여부 다름
                    Profile.LinearLimit.ContactDistance = 0.0f; // 보통 PxD6Joint에서 직접 사용 안 함

                    // Cone Limits (Swing - 기본적으로 45도 제한된 움직임)
                    Profile.ConeLimit.Swing1Motion = EAngularConstraintMotion::ACM_Limited;
                    Profile.ConeLimit.Swing2Motion = EAngularConstraintMotion::ACM_Limited;
                    Profile.ConeLimit.Swing1LimitDegrees = 10.0f;
                    Profile.ConeLimit.Swing2LimitDegrees = 10.0f;
                    Profile.ConeLimit.bSoftConstraint = true;
                    Profile.ConeLimit.Stiffness = 30.f;
                    Profile.ConeLimit.Damping = 3.f;
                    Profile.ConeLimit.Restitution = 0.0f; // PhysX 조인트 생성자에 따라 실제 적용 여부 다름
                    Profile.ConeLimit.ContactDistance = 0.0f;

                    // Twist Limits (기본적으로 45도 제한된 움직임)
                    Profile.TwistLimit.TwistMotion = EAngularConstraintMotion::ACM_Limited;
                    Profile.TwistLimit.TwistLimitDegrees = 10.0f; // -45도에서 +45도 사이
                    Profile.TwistLimit.bSoftConstraint = true;
                    Profile.TwistLimit.Stiffness = 30.f;
                    Profile.TwistLimit.Damping = 3.f;
                    Profile.TwistLimit.Restitution = 0.0f; // PhysX 조인트 생성자에 따라 실제 적용 여부 다름
                    Profile.TwistLimit.ContactDistance = 0.0f;

                    // 연결된 바디 간의 충돌 기본적으로 비활성화
                    Profile.bDisableCollision = true;

                    PhysicsAsset->ConstraintSetup.Add(ConstraintSetup);
                }
            }
        }
        return true;
    }

    // ELinearConstraintMotion을 PxD6Motion::Enum으로 매핑하는 헬퍼 함수
    physx::PxD6Motion::Enum MapLinearMotionToPx(ELinearConstraintMotion motion)
    {
        switch (motion)
        {
        case ELinearConstraintMotion::LCM_Free:   return physx::PxD6Motion::eFREE;
        case ELinearConstraintMotion::LCM_Limited:return physx::PxD6Motion::eLIMITED;
        case ELinearConstraintMotion::LCM_Locked: return physx::PxD6Motion::eLOCKED;
        default:                                  return physx::PxD6Motion::eLOCKED; // 기본값은 잠금
        }
    }
    // EAngularConstraintMotion을 PxD6Motion::Enum으로 매핑하는 헬퍼 함수
    physx::PxD6Motion::Enum MapAngularMotionToPx(EAngularConstraintMotion motion)
    {
        switch (motion)
        {
        case EAngularConstraintMotion::ACM_Free:    return physx::PxD6Motion::eFREE;
        case EAngularConstraintMotion::ACM_Limited: return physx::PxD6Motion::eLIMITED;
        case EAngularConstraintMotion::ACM_Locked:  return physx::PxD6Motion::eLOCKED;
        default:                                    return physx::PxD6Motion::eLOCKED; // 기본값은 잠금
        }
    }
}
