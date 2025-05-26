#include "PhysicsAssetUtils.h"

#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UObject/ObjectFactory.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicalMaterials/Defines.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

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

        assert(SkelMesh);
        assert(SkelMesh->GetRenderData());

        // 1. Bone들을 순회하면서 BodySetup 생성 -> 이거 너무 많이생기는데
        if (!SkelMesh->GetRefSkeleton())
            return false; // Skeleton이 없으면 실패

        // 일단 루트에다가만 한 개 만들기
        const FReferenceSkeleton* ReferenceSkeleton = SkelMesh->GetRefSkeleton();
        for (const auto& Bone : ReferenceSkeleton->GetRawRefBoneInfo())
        {
            if (Bone.ParentIndex == -1)
            {
                UBodySetup* BodySetup = FObjectFactory::ConstructObject<UBodySetup>(PhysicsAsset, FName(*FString::Printf(TEXT("BodySetup_%s"), *Bone.Name.ToString())));
                BodySetup->BoneName = Bone.Name;

                BodySetup->CollisionResponse = EBodyCollisionResponse::Type::BodyCollision_Enabled;
                BodySetup->DefaultInstance.ExternalCollisionProfileBodySetup = BodySetup;

                FPhysicsMaterial* PhysicsMaterial = new FPhysicsMaterial();
                PhysicsMaterial->Density = 1000.f; // 임시 밀도 값

                // !TODO : 디폴트 머티리얼을 하나 만들어두고 그걸 사용하도록 해야 함
                UPhysicalMaterial* PhysMaterial = FObjectFactory::ConstructObject<UPhysicalMaterial>(nullptr);
                PhysMaterial->Material = PhysicsMaterial;

                FKSphylElem SphylElem;

                BodySetup->AggGeom.SphylElems.Add(SphylElem);
                BodySetup->PhysMaterial = PhysMaterial;

                PhysicsAsset->BodySetup.Add(BodySetup);
                break;
            }
        }



        //for (int32 i = 0; i < SkelMesh->GetSkeleton()->GetReferenceSkeleton().GetRawBoneNum(); ++i)
        //{
        //    FName BoneName = SkelMesh->GetRefSkeleton()->GetBoneName(i);
        //    if (PhysicsAsset->FindBodyIndex(BoneName) != INDEX_NONE)
        //        continue; // 이미 존재하는 BodySetup은 건너뜀

        //    FString BodySetupName = FString::Printf(TEXT("BodySetup_%s"), *BoneName.ToString());
        //    UBodySetup* BodySetup = FObjectFactory::ConstructObject<UBodySetup>(PhysicsAsset, FName(BodySetupName));
        //    BodySetup->BoneName = BoneName;
        //    PhysicsAsset->BodySetup.Add(BodySetup);
        //}

        // 2. TODO..

    }
}
