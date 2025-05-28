#include "PhysicsAsset.h"

#include "BodySetup.h"
#include "PhysicsConstraintTemplate.h"
#include "Engine/AssetManager.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/ObjectFactory.h"

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

void UPhysicsAsset::BodyFindConstraints(int32 BodyIndex, TArray<int32>& Constraints)
{
    Constraints.Empty();
    FName BodyName = BodySetup[BodyIndex]->BoneName;

    for(int32 ConIdx=0; ConIdx<ConstraintSetup.Num(); ConIdx++)
    {
        if( ConstraintSetup[ConIdx]->DefaultInstance.ConstraintBone1 == BodyName || ConstraintSetup[ConIdx]->DefaultInstance.ConstraintBone2 == BodyName )
        {
            Constraints.Add(ConIdx);
        }
    }
}

void UPhysicsAsset::SerializeAsset(FArchive& Ar)
{
    Super::SerializeAsset(Ar);
    
    Ar << ConstraintSetup;
    Ar << BodySetup;
    Ar << CurrentConstraintProfileName;
    Ar << ConstraintProfiles;

    FName MeshName = NAME_None;
    TArray<FName> WeldParents;
    if (Ar.IsSaving())
    {
        MeshName = FName(PreviewSkeletalMesh->GetRenderData()->ObjectName);
    }
    
    Ar << MeshName;
    
    if (Ar.IsLoading())
    {
        PreviewSkeletalMesh = UAssetManager::Get().GetSkeletalMesh(MeshName);
    }


    PreviewSkeletalMesh->SetPhysicsAsset(this);
}
