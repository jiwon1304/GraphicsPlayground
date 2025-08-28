
#include "Skeleton.h"

FReferenceSkeleton USkeleton::GetReferenceSkeleton() const
{
    return ReferenceSkeleton;
}

const FReferenceSkeleton& USkeleton::GetReferenceSkeleton()
{
    return ReferenceSkeleton;
}

const TArray<FTransform>& USkeleton::GetReferencePose() const
{
    return ReferenceSkeleton.RawRefBonePose;
}

int32 USkeleton::FindBoneIndex(const FName& BoneName) const
{
    return ReferenceSkeleton.FindBoneIndex(BoneName);
}

void USkeleton::SerializeAsset(FArchive& Ar)
{
    ReferenceSkeleton.Serialize(Ar);
}
