#include "PhysicsAsset.h"

#include "BodySetup.h"

int32 UPhysicsAsset::FindBodyIndex(const FName& BoneName) const
{
    const int32* IdxData = BoneNameToIndexMap.Find(BoneName);
    if (IdxData)
    {
        return *IdxData;
    }

    return INDEX_NONE;
}

FName UPhysicsAsset::FindConstraintBoneName(int32 BoneIndex) const
{
    // !TODO : 미구현
    return FName();
}

void UPhysicsAsset::UpdateBodySetupIndexMap()
{
    // update BodySetupIndexMap
    BoneNameToIndexMap.Empty();

    for(int32 i=0; i<BodySetup.Num(); i++)
    {
        if (BodySetup[i])
        {
            BoneNameToIndexMap.Add(BodySetup[i]->BoneName, i);
        }
    }
}
