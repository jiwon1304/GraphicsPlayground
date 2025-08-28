#pragma once
#include <extensions/PxD6Joint.h>
#include <PhysicsEngine/ConstraintTypes.h>

class USkeletalMesh;
class UPhysicsAsset;

namespace FPhysicsAssetUtils
{
    bool HasSufficientSkinnedVertices(USkeletalMesh* SkelMesh, int32 BoneIndex, float MintotalWeightThreshold);
    bool IsboneLargeEnough(USkeletalMesh* SkelMesh, int32 BoneIndex, float MinBoneSizeThreshold);
    bool CreateFromSkeletalMesh(UPhysicsAsset* PhysicsAsset, USkeletalMesh* SkelMesh);
    physx::PxD6Motion::Enum MapLinearMotionToPx(ELinearConstraintMotion motion);
    physx::PxD6Motion::Enum MapAngularMotionToPx(EAngularConstraintMotion motion);
}
