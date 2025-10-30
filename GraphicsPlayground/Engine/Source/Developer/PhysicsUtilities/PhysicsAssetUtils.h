#pragma once
#include "Classes/PhysicsEngine/ConstraintTypes.h"

class USkeletalMesh;
class UPhysicsAsset;

namespace FPhysicsAssetUtils
{
    bool HasSufficientSkinnedVertices(USkeletalMesh* SkelMesh, int32 BoneIndex, float MintotalWeightThreshold);
    bool IsboneLargeEnough(USkeletalMesh* SkelMesh, int32 BoneIndex, float MinBoneSizeThreshold);
    bool CreateFromSkeletalMesh(UPhysicsAsset* PhysicsAsset, USkeletalMesh* SkelMesh);
}
