#include "PhysicsAssetUtils.h"

#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"
namespace FPhysicsAssetUtils
{
// bool CreateFromSkeletalMeshInternal(UPhysicsAsset* PhysicsAsset, USkeletalMesh* SkelMesh, const FPhysAssetCreateParams& Params, const FSkinnedBoneTriangleCache& TriangleCache, bool bShowProgress)
// {
// // For each bone, get the vertices most firmly attached to it.
// TArray<FBoneVertInfo> Infos;
// FMeshUtilitiesEngine::CalcBoneVertInfos(SkelMesh, Infos, (Params.VertWeight == EVW_DominantWeight));
// assert(Infos.Num() == SkelMesh->GetSkeleton()->GetReferenceSkeleton().GetRawBoneNum());
//
// PhysicsAsset->CollisionDisableTable.Empty();
//
// //Given the desired min body size we work from the children up to "merge" bones together. We go from leaves up because usually fingers, toes, etc... are small bones that should be merged
// //The strategy is as follows:
// //If bone is big enough, make a body
// //If not, add bone to parent for possible merge
//
// const TArray<FTransform> LocalPose = SkelMesh->GetSkeleton()->GetReferenceSkeleton().GetRawRefBonePose();
// typedef TArray<int32> FMergedBoneIndices;
//
// TMap<int32, TPair<FMergedBoneIndices, FBoneVertInfo>> BoneToMergedBones;
// const int32 NumBones = Infos.Num();
//
// TArray<float> MergedSizes;
// MergedSizes.AddZeroed(NumBones);
// for(int32 BoneIdx = NumBones-1; BoneIdx >=0; --BoneIdx)
// {
//     const float MyMergedSize = MergedSizes[BoneIdx] += CalcBoneInfoLength(Infos[BoneIdx]);
//
//     if(MyMergedSize < Params.MinBoneSize && MyMergedSize >= Params.MinWeldSize)
//     {
//         //Too small to make a body for, so let's merge with parent bone. TODO: use a merge threshold
//         const int32 ParentIndex = SkelMesh->GetRefSkeleton().GetParentIndex(BoneIdx);
//         if(ParentIndex != INDEX_NONE)
//         {
//             MergedSizes[ParentIndex] += MyMergedSize;
//             TPair<FMergedBoneIndices,FBoneVertInfo>& ParentMergedBones = BoneToMergedBones.FindOrAdd(ParentIndex);	//Add this bone to its parent merged bones
//             ParentMergedBones.Get<0>().Add(BoneIdx);
//
//             const FTransform LocalTM = LocalPose[BoneIdx];
//
//             AddInfoToParentInfo(LocalTM, Infos[BoneIdx], ParentMergedBones.Get<1>());
//
//             if(TPair<FMergedBoneIndices, FBoneVertInfo>* MyMergedBones = BoneToMergedBones.Find(BoneIdx))
//             {
//                 //make sure any bones merged to this bone get merged into the parent
//                 ParentMergedBones.Get<0>().Append(MyMergedBones->Get<0>());
//                 AddInfoToParentInfo(LocalTM, MyMergedBones->Get<1>(), ParentMergedBones.Get<1>());
//
//                 BoneToMergedBones.Remove(BoneIdx);
//             }
//         }
//     }
// }
//
// //We must ensure that there is a single root body no matter how small.
// int32 ForcedRootBoneIndex = INDEX_NONE;
// int32 FirstParentBoneIndex = INDEX_NONE;
// for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
// {
//     if (MergedSizes[BoneIndex] > Params.MinBoneSize)
//     {
//         const int32 ParentBoneIndex = SkelMesh->GetRefSkeleton().GetParentIndex(BoneIndex);
//         if(ParentBoneIndex == INDEX_NONE)
//         {
//             break;	//We already have a single root body, so don't worry about it
//         }
//         else if(FirstParentBoneIndex == INDEX_NONE)
//         {
//             FirstParentBoneIndex = ParentBoneIndex;	//record first parent to see if we have more than one root
//         }
//         else if(ParentBoneIndex == FirstParentBoneIndex)
//         {
//             ForcedRootBoneIndex = ParentBoneIndex;	//we have two "root" bodies so take their parent as the real root body
//             break;
//         }
//     }
// }
//
// auto ShouldMakeBone = [&Params, &MergedSizes, ForcedRootBoneIndex](int32 BoneIndex)
// {
//     // If desired - make a body for EVERY bone
//     if (Params.bBodyForAll)
//     {
//         return true;
//     }
//     else if (MergedSizes[BoneIndex] > Params.MinBoneSize)
//     {
//         // If bone is big enough - create physics.
//         return true;
//     }
//     else if (BoneIndex == ForcedRootBoneIndex)
//     {
//         // If the bone is a forced root body we must create they body no matter how small
//         return true;
//     }
//
//     return false;
// };
//
// int32 RootBoneIndex = 0;
// if (Params.GeomType == EFG_SkinnedLevelSet)
// {
//     // Calculate common root bone from all merged bones
//     if (!Params.bBodyForAll)
//     {
//         TArray<int32> MergedBones;
//         MergedBones.Reserve(NumBones);
//         for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
//         {
//             if (ShouldMakeBone(BoneIndex))
//             {
//                 MergedBones.Add(BoneIndex);
//             }
//         }
//         RootBoneIndex = CalculateCommonRootBoneIndex(SkelMesh->GetRefSkeleton(), MergedBones);
//     }
// }
//
// FSkinnedLevelSetBuilder LatticeBuilder(*SkelMesh, TriangleCache, RootBoneIndex);
// if (Params.GeomType == EFG_SkinnedLevelSet)
// {
//     check(RootBoneIndex != INDEX_NONE);
//     TArray<int32> AllBoneIndices;
//     const int32 RawBoneNum = SkelMesh->GetRefSkeleton().GetRawBoneNum();
//     AllBoneIndices.Reserve(RawBoneNum - RootBoneIndex);
//     for (int32 BoneIndex = RootBoneIndex; BoneIndex < RawBoneNum; ++BoneIndex)
//     {
//         AllBoneIndices.Emplace(BoneIndex);
//     }
//     TArray<uint32> OrigIndicesUnused;
//     if (!LatticeBuilder.InitializeSkinnedLevelset(Params, AllBoneIndices, OrigIndicesUnused))
//     {
//         return false;
//     }
// }
//
// // Finally, iterate through all the bones and create bodies when needed
//
// const bool bCanCreateConstraints = CanCreateConstraints();
// FScopedSlowTask SlowTask((float)NumBones * 2, FText(), bShowProgress&& IsInGameThread());
// if (bShowProgress && IsInGameThread())
// {
//     SlowTask.MakeDialog();
// }
//
// for (int32 BoneIndex = 0; BoneIndex < NumBones; BoneIndex++)
// {
//     // Determine if we should create a physics body for this bone
//     const bool bMakeBone = ShouldMakeBone(BoneIndex);
//
//     if (bMakeBone)
//     {
//         // Go ahead and make this bone physical.
//         FName BoneName = SkelMesh->GetRefSkeleton().GetBoneName(BoneIndex);
//
//         if (bShowProgress && IsInGameThread())
//         {
//             SlowTask.EnterProgressFrame(1.0f, FText::Format(NSLOCTEXT("PhysicsAssetEditor", "ResetCollsionStepInfo", "Generating collision for {0}"), FText::FromName(BoneName)));
//         }
//
//
//         //Construct the info - in the case of merged bones we append all the data
//         FBoneVertInfo Info = Infos[BoneIndex];
//         FMergedBoneIndices BoneIndices;
//         BoneIndices.Add(BoneIndex);
//         if (const TPair<FMergedBoneIndices, FBoneVertInfo>* MergedBones = BoneToMergedBones.Find(BoneIndex))
//         {
//             //Don't need to convert into parent space since this was already done
//             BoneIndices.Append(MergedBones->Get<0>());
//             Info.Normals.Append(MergedBones->Get<1>().Normals);
//             Info.Positions.Append(MergedBones->Get<1>().Positions);
//         }
//
//         if (Params.GeomType == EFG_SkinnedLevelSet)
//         {
//             LatticeBuilder.AddBoneInfluence(BoneIndex, BoneIndices);
//         }
//         else
//         {
//             const int32 NewBodyIndex = CreateNewBody(PhysicsAsset, BoneName, Params);
//             UBodySetup* NewBodySetup = PhysicsAsset->SkeletalBodySetups[NewBodyIndex];
//             check(NewBodySetup->BoneName == BoneName);
//
//             // Fill in collision info for this bone.
//             const bool bSuccess = CreateCollisionFromBoneInternal(NewBodySetup, SkelMesh, BoneIndex, Params, Info, TriangleCache);
//             if (bSuccess)
//             {
//                 // create joint to parent body
//                 if (Params.bCreateConstraints && bCanCreateConstraints)
//                 {
//                     int32 ParentIndex = BoneIndex;
//                     int32 ParentBodyIndex = INDEX_NONE;
//                     FName ParentName;
//
//                     do
//                     {
//                         //Travel up the hierarchy to find a parent which has a valid body
//                         ParentIndex = SkelMesh->GetRefSkeleton().GetParentIndex(ParentIndex);
//                         if (ParentIndex != INDEX_NONE)
//                         {
//                             ParentName = SkelMesh->GetRefSkeleton().GetBoneName(ParentIndex);
//                             ParentBodyIndex = PhysicsAsset->FindBodyIndex(ParentName);
//                         }
//                         else
//                         {
//                             //no more parents so just stop
//                             break;
//                         }
//
//                     } while (ParentBodyIndex == INDEX_NONE);
//
//                     if (ParentBodyIndex != INDEX_NONE)
//                     {
//                         //found valid parent body so create joint
//                         int32 NewConstraintIndex = CreateNewConstraint(PhysicsAsset, BoneName);
//                         UPhysicsConstraintTemplate* CS = PhysicsAsset->ConstraintSetup[NewConstraintIndex];
//
//                         // set angular constraint mode
//                         CS->DefaultInstance.SetAngularSwing1Motion(Params.AngularConstraintMode);
//                         CS->DefaultInstance.SetAngularSwing2Motion(Params.AngularConstraintMode);
//                         CS->DefaultInstance.SetAngularTwistMotion(Params.AngularConstraintMode);
//
//                         // Place joint at origin of child
//                         CS->DefaultInstance.ConstraintBone1 = BoneName;
//                         CS->DefaultInstance.ConstraintBone2 = ParentName;
//                         CS->DefaultInstance.SnapTransformsToDefault(EConstraintTransformComponentFlags::All, PhysicsAsset);
//
//                         CS->SetDefaultProfile(CS->DefaultInstance);
//
//                         // Disable collision between constrained bodies by default.
//                         PhysicsAsset->DisableCollision(NewBodyIndex, ParentBodyIndex);
//                     }
//                 }
//             }
//             else
//             {
//                 DestroyBody(PhysicsAsset, NewBodyIndex);
//             }
//         }
//     }
// }
//
// if (Params.GeomType == EFG_SkinnedLevelSet)
// {
//     // Finish Building WeightedLattice
//     const FName RootBoneName = SkelMesh->GetRefSkeleton().GetBoneName(RootBoneIndex);
//     const int32 NewBodyIndex = CreateNewBody(PhysicsAsset, RootBoneName, Params);
//     UBodySetup* NewBodySetup = PhysicsAsset->SkeletalBodySetups[NewBodyIndex];
//     CreateWeightedLevelSetBody(LatticeBuilder, NewBodySetup, PhysicsAsset, RootBoneName, Params);
// }
//
// //Go through and ensure any overlapping bodies are marked as disable collision
// FPreviewScene TmpScene;
// UWorld* TmpWorld = TmpScene.GetWorld();
// ASkeletalMeshActor* SkeletalMeshActor = TmpWorld->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), FTransform::Identity);
// SkeletalMeshActor->GetSkeletalMeshComponent()->SetSkeletalMesh(SkelMesh);
// USkeletalMeshComponent* SKC = SkeletalMeshActor->GetSkeletalMeshComponent();
// SKC->SetPhysicsAsset(PhysicsAsset);
// SkeletalMeshActor->RegisterAllComponents();
//
//
// const TArray<FBodyInstance*> Bodies = SKC->Bodies;
// const int32 NumBodies = Bodies.Num();
// for(int32 BodyIdx = 0; BodyIdx < NumBodies; ++BodyIdx)
// {
//     FBodyInstance* BodyInstance = Bodies[BodyIdx];
//     if(BodyInstance && BodyInstance->BodySetup.IsValid())
//     {
//         if (bShowProgress && IsInGameThread())
//         {
//             SlowTask.EnterProgressFrame(1.0f, FText::Format(NSLOCTEXT("PhysicsAssetEditor", "ResetCollsionStepInfoOverlaps", "Fixing overlaps for {0}"), FText::FromName(BodyInstance->BodySetup->BoneName)));
//         }
//
//         FTransform BodyTM = BodyInstance->GetUnrealWorldTransform();
//
//         for(int32 OtherBodyIdx = BodyIdx + 1; OtherBodyIdx < NumBodies; ++OtherBodyIdx)
//         {
//             FBodyInstance* OtherBodyInstance = Bodies[OtherBodyIdx];
//             if(OtherBodyInstance && OtherBodyInstance->BodySetup.IsValid())
//             {
//                 if(BodyInstance->OverlapTestForBody(BodyTM.GetLocation(), BodyTM.GetRotation(), OtherBodyInstance))
//                 {
//                     PhysicsAsset->DisableCollision(BodyIdx, OtherBodyIdx);
//                 }
//             }
//         }
//     }
// }
//
// return NumBodies > 0;
// }

bool CreateFromSkeletalMesh(UPhysicsAsset* PhysicsAsset, USkeletalMesh* SkelMesh)
{
    return false;
    
    PhysicsAsset->PreviewSkeletalMesh = SkelMesh;

    assert(SkelMesh);
    assert(SkelMesh->GetRenderData());

    // FSkinnedBoneTriangleCache TriangleCache(*SkelMesh, Params);

    // if (Params.GeomType == EFG_SingleConvexHull || Params.GeomType == EFG_MultiConvexHull || Params.GeomType == EFG_LevelSet || Params.GeomType == EFG_SkinnedLevelSet)
    // {
    //     TriangleCache.BuildCache();
    // }

    // bool bSuccess = CreateFromSkeletalMeshInternal(PhysicsAsset, SkelMesh, Params, TriangleCache, bShowProgress);
    // if (!bSuccess)
    // {
    //     // try lower minimum bone size 
    //     FPhysAssetCreateParams LocalParams = Params;
    //     LocalParams.MinBoneSize = 1.f;
    //
    //     bSuccess = CreateFromSkeletalMeshInternal(PhysicsAsset, SkelMesh, LocalParams, TriangleCache, bShowProgress);
    //
    //     if(!bSuccess)
    //     {
    //         MessageBoxA(nullptr, "Failed to Create PhysicsAsset", "Error", MB_OK | MB_ICONERROR);
    //     }
    // }

    // if (bSuccess)
    // {
    //     SkelMesh->SetPhysicsAsset(PhysicsAsset);
    // }
    //
    // return bSuccess;
}
}
