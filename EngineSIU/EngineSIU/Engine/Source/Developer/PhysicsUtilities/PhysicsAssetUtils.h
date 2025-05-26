#pragma once

class USkeletalMesh;
class UPhysicsAsset;

namespace FPhysicsAssetUtils
{
    bool CreateFromSkeletalMesh(UPhysicsAsset* PhysicsAsset, USkeletalMesh* SkelMesh);
}
