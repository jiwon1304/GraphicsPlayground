#include "PhysicsAssetViewerRenderPass.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World/PhysicsAssetWorld.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"

FPhysicsAssetViewerRenderPass::FPhysicsAssetViewerRenderPass()
{
    Super::FOverlayShapeRenderPass();
}

FPhysicsAssetViewerRenderPass::~FPhysicsAssetViewerRenderPass()
{
    Super::~FOverlayShapeRenderPass();
}

void FPhysicsAssetViewerRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    Super::Initialize(InBufferManager, InGraphics, InShaderManager);
}

void FPhysicsAssetViewerRenderPass::PrepareRenderArr()
{
    ClearRenderArr();
    Super::PrepareRenderArr();
}

void FPhysicsAssetViewerRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    UPhysicsAssetWorld* PhysicsAssetWorld = Cast<UPhysicsAssetWorld>(GEngine->ActiveWorld);
    if (!PhysicsAssetWorld)
    {
        return;
    }

    USkeletalMeshComponent* SkelComp = PhysicsAssetWorld->GetSkeletalMeshComponent();
    if (!SkelComp)
    {
        return;
    }
    
    RenderSkelComp(SkelComp);

    Super::Render(Viewport);
}

void FPhysicsAssetViewerRenderPass::ClearRenderArr()
{
    Super::ClearRenderArr();
}

void FPhysicsAssetViewerRenderPass::RenderSkelComp(USkeletalMeshComponent* SkelComp)
{
    USkeletalMesh* SkeletalMesh = SkelComp->GetSkeletalMeshAsset();
    if (!SkeletalMesh)
    {
        return;
    }

    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
    if (!PhysicsAsset)
    {
        return;
    }
    // Bone의 정보 얻기
    const FReferenceSkeleton* ReferenceSkeleton = SkeletalMesh->GetRefSkeleton();
    const TArray<FMeshBoneInfo>& RawBoneInfo = ReferenceSkeleton->GetRawRefBoneInfo();
    const TArray<FTransform>& RefBonePoses = SkeletalMesh->GetRefSkeleton()->GetRawRefBonePose();

    for (UBodySetup* BodySetup : PhysicsAsset->BodySetup)
    {
        FName BoneName = BodySetup->BoneName;
        int32 BoneIndex = ReferenceSkeleton->FindBoneIndex(BoneName);
        if (BoneIndex == INDEX_NONE)
        {
            continue; // 해당 Bone이 없으면 건너뜀
        }

        FMatrix InitialMatrix = SkelComp->GetBoneComponentSpaceTransform(BoneIndex).ToMatrixNoScale() 
            * SkelComp->GetWorldMatrix();

        FVector InitialPosition = InitialMatrix.GetTranslationVector();
        FQuat InitialRotation = InitialMatrix.ToQuat();

        FTransform InitialTransform(InitialRotation, InitialPosition);

        FKAggregateGeom AggGeom = BodySetup->AggGeom;

        for (FKSphereElem& SphereElem : AggGeom.SphereElems)
        {
            SphereElem.Center = InitialPosition;
            Shape::FSphere Sphere(SphereElem.Center, SphereElem.Radius);
            Spheres.Add(TPair<Shape::FSphere, FLinearColor>(Sphere, FLinearColor(1, 0, 0, 0.2)));
        }

        for (FKBoxElem& BoxElem : AggGeom.BoxElems)
        {
            BoxElem.SetTransform(InitialTransform);
            Shape::FOrientedBox OrientedBox = BoxElem.ToFOrientedBox();
            OrientedBoxes.Add(TPair<Shape::FOrientedBox, FLinearColor>(OrientedBox, FLinearColor(0, 1, 0, 0.2)));
        }

        for (FKSphylElem& SphylElem : AggGeom.SphylElems)
        {
            SphylElem.SetTransform(InitialTransform);
            Shape::FCapsule Capsule = SphylElem.ToFCapsule();
            Capsules.Add(TPair<Shape::FCapsule, FLinearColor>(Capsule, FLinearColor(1, 0, 1, 0.5)));
        }
    }
}
