#include "AnimDataModel.h"

#include "Classes/Animation/AnimationAsset.h"
#include "Classes/Animation/AnimSequence.h"
#include "Classes/Animation/AnimTypes.h"
#include "Container/Array.h"
#include "Developer/AnimDataController/AnimDataController.h"
#include "Math/Transform.h"
#include "Misc/FrameTime.h"
#include "CoreUObject/UObject/Casts.h"
#include "CoreUObject/UObject/ObjectFactory.h"

UAnimDataModel::UAnimDataModel()
{
}

const TArray<FBoneAnimationTrack>& UAnimDataModel::GetBoneAnimationTracks() const
{
    return Data.BoneAnimationTracks;
}

const FBoneAnimationTrack& UAnimDataModel::GetBoneTrackByIndex(int32 TrackIndex) const
{
    return Data.BoneAnimationTracks[TrackIndex];
}

const FBoneAnimationTrack& UAnimDataModel::GetBoneTrackByName(const FName& TrackName) const
{
    const FBoneAnimationTrack* TrackPtr = Data.BoneAnimationTracks.FindByPredicate([TrackName](const FBoneAnimationTrack& Track)
    {
        return Track.Name == TrackName;
    });

	return *TrackPtr;
}

const FBoneAnimationTrack* UAnimDataModel::FindBoneTrackByName(FName Name) const
{
    return Data.BoneAnimationTracks.FindByPredicate([Name](const FBoneAnimationTrack& Track)
    {
        return Track.Name == Name;
    });
}

const FBoneAnimationTrack* UAnimDataModel::FindBoneTrackByIndex(int32 BoneIndex) const
{
    const FBoneAnimationTrack* TrackPtr = Data.BoneAnimationTracks.FindByPredicate([BoneIndex](const FBoneAnimationTrack& Track)
	{
		return Track.BoneTreeIndex == BoneIndex;
	});

	return TrackPtr;
}

int32 UAnimDataModel::GetBoneTrackIndex(const FBoneAnimationTrack& Track) const
{
    return Data.BoneAnimationTracks.IndexOfByPredicate([&Track](const FBoneAnimationTrack& SearchTrack)
    {
        return SearchTrack.Name == Track.Name;
    });
}

int32 UAnimDataModel::GetBoneTrackIndexByName(FName TrackName) const
{
    if (const FBoneAnimationTrack* TrackPtr = FindBoneTrackByName(TrackName))
    {
        return GetBoneTrackIndex(*TrackPtr);
    }

    return INDEX_NONE;
}

bool UAnimDataModel::IsValidBoneTrackIndex(int32 TrackIndex) const
{
    return Data.BoneAnimationTracks.IsValidIndex(TrackIndex);
}

FTransform UAnimDataModel::EvaluateBoneTrackTransform(FName TrackName, const FFrameTime& FrameTime, const EAnimInterpolationType& Interpolation) const
{
    const float Alpha = Interpolation == EAnimInterpolationType::Step ? FMath::RoundToFloat(FrameTime.GetSubFrame()) : FrameTime.GetSubFrame();

    if (FMath::IsNearlyEqual(Alpha, 1.0f))
    {
        return GetBoneTrackTransform(TrackName, FrameTime.CeilToFrame());
    }
    else if (FMath::IsNearlyZero(Alpha))
    {
        return GetBoneTrackTransform(TrackName, FrameTime.FloorToFrame());
    }
	
    const FTransform From = GetBoneTrackTransform(TrackName, FrameTime.FloorToFrame());
    const FTransform To = GetBoneTrackTransform(TrackName, FrameTime.CeilToFrame());

    FTransform Blend;
    Blend.Blend(From, To, Alpha);
    return Blend;
}

FTransform UAnimDataModel::GetBoneTrackTransform(FName TrackName, const int32& FrameNumber) const
{
    const FBoneAnimationTrack* Track = GetBoneAnimationTracks().FindByPredicate([TrackName](const FBoneAnimationTrack& Track)
    {
        return Track.Name == TrackName;
    });

    if (Track)
    {
        const int32 KeyIndex = FrameNumber;
        if (Track->InternalTrackData.PosKeys.IsValidIndex(KeyIndex) &&
            Track->InternalTrackData.RotKeys.IsValidIndex(KeyIndex) &&
            Track->InternalTrackData.ScaleKeys.IsValidIndex(KeyIndex))
        {
            return FTransform(FQuat(Track->InternalTrackData.RotKeys[KeyIndex]),
                            FVector(Track->InternalTrackData.PosKeys[KeyIndex]),
                            FVector(Track->InternalTrackData.ScaleKeys[KeyIndex])
            );
        }
    }

    return FTransform::Identity;
}

void UAnimDataModel::GetBoneTrackTransforms(FName TrackName, const TArray<int32>& FrameNumbers, TArray<FTransform>& OutTransforms) const
{
    const FBoneAnimationTrack* Track = GetBoneAnimationTracks().FindByPredicate([TrackName](const FBoneAnimationTrack& Track)
    {
        return Track.Name == TrackName;
    });

    OutTransforms.SetNum(FrameNumbers.Num());

    if (Track)
    {
        for (int32 EntryIndex = 0; EntryIndex < FrameNumbers.Num(); ++EntryIndex)
        {
            OutTransforms[EntryIndex] = GetBoneTrackTransform(TrackName, FrameNumbers[EntryIndex]);
        }
    }
}

void UAnimDataModel::GetBoneTrackTransforms(FName TrackName, TArray<FTransform>& OutTransforms) const
{
    const FBoneAnimationTrack* Track = GetBoneAnimationTracks().FindByPredicate([TrackName](const FBoneAnimationTrack& Track)
    {
        return Track.Name == TrackName;
    });
	
    OutTransforms.SetNum(Data.NumberOfKeys);

    if (Track)
    {
        for (int32 KeyIndex = 0; KeyIndex < Data.NumberOfKeys; ++KeyIndex)
        {
            OutTransforms[KeyIndex] = GetBoneTrackTransform(TrackName, KeyIndex);
        }
    }
}

void UAnimDataModel::GetBoneTracksTransform(const TArray<FName>& TrackNames, const int32& FrameNumber, TArray<FTransform>& OutTransforms) const
{
    OutTransforms.SetNum(TrackNames.Num());
    for (int32 EntryIndex = 0; EntryIndex < TrackNames.Num(); ++EntryIndex)
    {
        OutTransforms[EntryIndex] = GetBoneTrackTransform(TrackNames[EntryIndex], FrameNumber);
    }
}

int32 UAnimDataModel::GetNumBoneTracks() const
{
    return Data.BoneAnimationTracks.Num();
}

void UAnimDataModel::GetBoneTrackNames(TArray<FName>& OutNames) const
{
    OutNames.Empty();
    
    for (const FBoneAnimationTrack& Track : Data.BoneAnimationTracks)
    {
        OutNames.Add(Track.Name);
    }
}

double UAnimDataModel::GetPlayLength() const
{
    return Data.NumberOfFrames * Data.FrameRate;
}

int32 UAnimDataModel::GetNumberOfFrames() const
{
    return Data.NumberOfFrames;
}

int32 UAnimDataModel::GetNumberOfKeys() const
{
    return Data.NumberOfKeys;
}

double UAnimDataModel::GetFrameRate() const
{
    return Data.FrameRate;
}

UAnimSequence* UAnimDataModel::GetAnimationSequence() const
{
    if (UAnimSequence* AnimationSequence = Cast<UAnimSequence>(GetOuter()))
    {
        return AnimationSequence;
    }
    return nullptr;
}

UAnimDataController* UAnimDataModel::GetController()
{
    UAnimDataController* Controller = FObjectFactory::ConstructObject<UAnimDataController>(GetOuter());
    Controller->SetModel(this);
    return Controller;
}

USkeleton* UAnimDataModel::GetSkeleton() const
{
    if (const UAnimationAsset* AnimationAsset = Cast<const UAnimationAsset>(GetOuter()))
    {
        if (USkeleton* Skeleton = AnimationAsset->GetSkeleton())
        {
            return Skeleton;
        }
    }
    return nullptr;
}

FBoneAnimationTrack* UAnimDataModel::FindMutableBoneTrackByName(FName Name)
{
    return Data.BoneAnimationTracks.FindByPredicate([Name](const FBoneAnimationTrack& Track)
    {
        return Track.Name == Name;
    });
}
