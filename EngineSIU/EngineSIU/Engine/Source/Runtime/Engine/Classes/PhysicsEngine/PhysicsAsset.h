#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"

// 하나의 랙돌을 구성하는 rigid body와 constraint의 집합체입니다. 하나의 SkeletalMesh는 하나의 PhysicsAsset를 가집니다.
class UPhysicsAsset : public UObject
{
    DECLARE_CLASS(UPhysicsAsset, UObject)

public:
    UPROPERTY(
        EditAnywhere,
        class USkeletalMesh*,
        PreviewSkeletalMesh,
        = nullptr
    )

    UPROPERTY(
        EditAnywhere,
        TArray<FName>,
        ConstraintProfiles,
    )

    UPROPERTY(
        EditAnywhere,
        FName,
        CurrentConstraintProfileName,
    )

    UPROPERTY(
        TArray<class UBodySetup*>,
        BodySetups
    )

    UPROPERTY(
        TArray<class UPhysicsConstraintTemplate*>,
        ConstraintSetup
    )

    // Name대신 index를 캐싱해서 사용
    TMap<FName, int32> BoneNameToIndexMap;

    UPhysicsAsset() = default;
    // PhysicsAsset의 Bone 이름을 index로 매핑합니다.
    // PhysicsAsset의 Bone 이름을 index로 가져옵니다.
    int32 FindBodyIndex(const FName& BoneName) const;
    // PhysicsAsset의 Bone 이름을 가져옵니다.
    FName FindConstraintBoneName(int32 BoneIndex) const;

    void UpdateBodySetupIndexMap();
};
