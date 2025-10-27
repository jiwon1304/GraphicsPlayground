
#include "SkeletalMeshActorTest.h"

#include "Classes/Animation/AnimSequence.h"
#include "Classes/Animation/Skeleton.h"
#include "Classes/Components/SkeletalMeshComponent.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Classes/Engine/AssetManager.h"
#include "Classes/Engine/FObjLoader.h"
#include "Classes/Engine/SkeletalMesh.h"
#include "Classes/Engine/Asset/SkeletalMeshAsset.h"

ASkeletalMeshActorTest::ASkeletalMeshActorTest()
{
    SetActorTickInEditor(true);
}

void ASkeletalMeshActorTest::PostSpawnInitialize()
{
    AActor::PostSpawnInitialize();

    USceneComponent* Root = AddComponent<USceneComponent>(FName("RootComponent_0"));
    RootComponent = Root;

    MeshComp = AddComponent<USkeletalMeshComponent>(FName("SkeletalMeshComponent_0"));
    MeshComp->SetSkeletalMeshAsset(UAssetManager::Get().GetSkeletalMesh("Contents/FBX/Sharkry_Unreal"));
    MeshComp->SetupAttachment(RootComponent);

    
    if (MeshComp->GetSkeletalMeshAsset())
    {
        const FReferenceSkeleton& RefSkeleton = MeshComp->GetSkeletalMeshAsset()->GetSkeleton()->GetReferenceSkeleton();
        
        for (int32 Idx = 0; Idx < RefSkeleton.RawRefBoneInfo.Num(); ++Idx)
        {
            UStaticMeshComponent* Dot = AddComponent<UStaticMeshComponent>();
            Dot->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/SpherePrimitive.obj"));
            DotComponents.Add(Dot);
            
            int32 ParentIndex = RefSkeleton.RawRefBoneInfo[Idx].ParentIndex;
            if (ParentIndex != INDEX_NONE)
            {
                Dot->AttachToComponent(DotComponents[ParentIndex]);
            }
            else
            {
                Dot->AttachToComponent(RootComponent);
            }
            
            Dot->SetRelativeTransform(RefSkeleton.RawRefBonePose[Idx]);
            Dot->SetWorldScale3D(FVector(1.f));
        }
    }
    
}

void ASkeletalMeshActorTest::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);



}
