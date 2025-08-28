#include "PhysicsAssetViewerRenderPass.h"
#include "Container/Map.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World/PhysicsAssetWorld.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "Engine/EditorEngine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
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
    if (UPhysicsAssetWorld* PhysicsAssetWorld = Cast<UPhysicsAssetWorld>(GEngine->ActiveWorld))
    {
        USkeletalMeshComponent* SkelComp = PhysicsAssetWorld->GetSkeletalMeshComponent();
        if (!SkelComp)
        {
            return;
        }
    
        RenderSkelComp(SkelComp, true);

    }
    // 그냥 월드일경우
    else if (UWorld* World = GEngine->ActiveWorld)
    {
        for (UStaticMeshComponent* SMComp : TObjectRange<UStaticMeshComponent>())
        {
            if (!SMComp || SMComp->GetWorld() != World)
            {
                continue;
            }
            RenderStaticComp(SMComp, false);
        }

        for (USkeletalMeshComponent* SkelComp : TObjectRange<USkeletalMeshComponent>())
        {
            if (!SkelComp || SkelComp->GetWorld() != World)
            {
                continue;
            }
            RenderSkelComp(SkelComp, false);
        }
    }
    Super::Render(Viewport);

}

void FPhysicsAssetViewerRenderPass::ClearRenderArr()
{
    Super::ClearRenderArr();
}

void FPhysicsAssetViewerRenderPass::RenderStaticComp(UStaticMeshComponent* StaticComp, bool bPreviewWorld)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

    UStaticMesh* StaticMesh = StaticComp->GetStaticMesh();
    if (!StaticMesh)
    {
        return;
    }

    UBodySetup* BodySetup = StaticMesh->GetBodySetup();
    if (!BodySetup)
    {
        return;
    }

    FKAggregateGeom AggGeom = BodySetup->AggGeom;

    FLinearColor Color = FLinearColor(1, 0, 1, 0.3);

    if (!bPreviewWorld)
    {
        if (UEditorEngine* Engine = Cast<UEditorEngine>(GEngine))
        {
            if (Engine->GetSelectedActor() == StaticComp->GetOwner())
            {
                Color = FLinearColor(0, 1, 1, 0.3);
            }
        }
    }

    FMatrix InitialMatrix = StaticComp->GetWorldMatrix();
    InitialMatrix.GetMatrixWithoutScale();
    FVector InitialPosition = InitialMatrix.GetTranslationVector();
    FQuat InitialRotation = InitialMatrix.ToQuat();
    InitialRotation.Normalize();

    FTransform InitialTransform(InitialRotation, InitialPosition);

    for (FKSphereElem& SphereElem : AggGeom.SphereElems)
    {
        FTransform Src = FTransform(SphereElem.Center);
        FTransform Dst;
        Dst = InitialTransform * Src;

        SphereElem.Center = Dst.GetTranslation();
        Shape::FSphere Sphere(SphereElem.Center, SphereElem.Radius);
        Spheres.Add(TPair<Shape::FSphere, FLinearColor>(Sphere, Color));
    }

    for (FKBoxElem& BoxElem : AggGeom.BoxElems)
    {
        FQuat Rotation = BoxElem.Rotation.Quaternion();
        FTransform Src = { Rotation, BoxElem.Center };
        FTransform Dst;
        Dst = InitialTransform * Src;

        BoxElem.SetTransform(Dst);
        Shape::FOrientedBox OrientedBox = BoxElem.ToFOrientedBox();
        OrientedBoxes.Add(TPair<Shape::FOrientedBox, FLinearColor>(OrientedBox, Color));
    }

    for (FKSphylElem& SphylElem : AggGeom.SphylElems)
    {
        FQuat Rotation = SphylElem.Rotation.Quaternion();
        FTransform Src = { Rotation, SphylElem.Center };
        FTransform Dst;
        Dst = InitialTransform * Src;

        SphylElem.SetTransform(Dst);
        Shape::FCapsule Capsule = SphylElem.ToFCapsule();
        Capsules.Add(TPair<Shape::FCapsule, FLinearColor>(Capsule, Color));
    }


}

void FPhysicsAssetViewerRenderPass::RenderSkelComp(USkeletalMeshComponent* SkelComp, bool bPreviewWorld)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (!EditorEngine)
    {
        return;
    }

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

    // ----- [선택 정보 얻기] -----
    int32 SelectedBoneIndex = -1; 
    int32 SelectedBodySetupIndex = -1;
    int32 SelectedConstraintIndex = -1;

    int32 ParentBodySetupIndex = -1;
    int32 SelectedPrimitiveIndex = -1;
    EAggCollisionShape::Type PrimitiveType = EAggCollisionShape::Unknown;

    if (bPreviewWorld)
    {
        SelectedBoneIndex = EditorEngine->PhysicsAssetEditorWorld->SelectBoneIndex;
        SelectedBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
        SelectedConstraintIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedConstraintIndex;
        ParentBodySetupIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex;
        SelectedPrimitiveIndex = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex;
        PrimitiveType = EditorEngine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType;
    }

    UBodySetup* SelectedBodySetup = SelectedBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[SelectedBodySetupIndex];

    // 뼈의 유효성 체크
    bool bIsBoneValid = SelectedBoneIndex != -1 ?
        SkeletalMesh->GetSkeleton()->GetReferenceSkeleton().IsValidRawIndex(SelectedBoneIndex) : false;

    UBodySetup* ParentBodySetup = ParentBodySetupIndex == -1 ? nullptr : PhysicsAsset->BodySetup[ParentBodySetupIndex];
    UPhysicsConstraintTemplate* PhysicsConstraintTemplate = SelectedConstraintIndex == -1 ? nullptr : PhysicsAsset->ConstraintSetup[SelectedConstraintIndex];

    // Primitive 유효성 체크
    bool bIsPrimitiveExist = false;
    if (PrimitiveType == EAggCollisionShape::Box)
    {
        if (SelectedPrimitiveIndex >= 0 && ParentBodySetup && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.BoxElems.Num())
        {
            bIsPrimitiveExist = true;
        }
    }
    else if (PrimitiveType == EAggCollisionShape::Sphere)
    {
        if (SelectedPrimitiveIndex >= 0 && ParentBodySetup && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphereElems.Num())
        {
            bIsPrimitiveExist = true;
        }
    }
    else if (PrimitiveType == EAggCollisionShape::Sphyl)
    {
        if (SelectedPrimitiveIndex >= 0 && ParentBodySetup && SelectedPrimitiveIndex < ParentBodySetup->AggGeom.SphylElems.Num())
        {
            bIsPrimitiveExist = true;
        }
    }
    if (!bIsPrimitiveExist)
    {
        ParentBodySetup = nullptr;
    }

    UBodySetup* TargetBodySetup = nullptr;
    if (ParentBodySetup)
    {
        TargetBodySetup = ParentBodySetup;
    }
    else if (SelectedBodySetup)
    {
        TargetBodySetup = SelectedBodySetup;
    }

    // Bone의 정보 얻기
    const FReferenceSkeleton* ReferenceSkeleton = SkeletalMesh->GetRefSkeleton();
    const TArray<FMeshBoneInfo>& RawBoneInfo = ReferenceSkeleton->GetRawRefBoneInfo();
    const TArray<FTransform>& RefBonePoses = SkeletalMesh->GetRefSkeleton()->GetRawRefBonePose();

    // Joint의 축
    // 
    for (const FMeshBoneInfo& BoneInfo : RawBoneInfo)
    {
        FName BoneName = BoneInfo.Name;
        int32 BoneIndex = ReferenceSkeleton->FindBoneIndex(BoneName);
        if (BoneIndex == INDEX_NONE)
        {
            continue; // 해당 Bone이 없으면 건너뜀
        }

        FMatrix InitialMatrix = SkelComp->GetBoneComponentSpaceTransform(BoneIndex).ToMatrixNoScale()
            * SkelComp->GetWorldMatrix();

        FVector JointPosition = InitialMatrix.GetTranslationVector();
        FQuat JointRotation = InitialMatrix.ToQuat();

        Shape::FOrientedBox JointAxisBox;
        JointAxisBox.Center = JointPosition;
        JointAxisBox.AxisX = JointRotation.RotateVector(FVector(1, 0, 0));
        JointAxisBox.AxisY = JointRotation.RotateVector(FVector(0, 1, 0));
        JointAxisBox.AxisZ = JointRotation.RotateVector(FVector(0, 0, 1));

        const float BoxThickness = 0.2;
        JointAxisBox.ExtentX = BoxThickness;
        JointAxisBox.ExtentY = BoxThickness;
        JointAxisBox.ExtentZ = BoxThickness;

        const float AxisBoxLength = 0.4;

        // 각 축마다 offset + extent + color로 구별
        Shape::FOrientedBox XAxisBox = JointAxisBox;
        XAxisBox.ExtentX = AxisBoxLength; // X축의 길이
        XAxisBox.Center += XAxisBox.AxisX * JointAxisBox.ExtentX; // X축 방향으로 이동
        OrientedBoxes.Add(TPair<Shape::FOrientedBox, FLinearColor>(XAxisBox, FLinearColor(1, 0, 0, 0.5))); // 빨간색

        Shape::FOrientedBox YAxisBox = JointAxisBox;
        YAxisBox.ExtentY = AxisBoxLength; // Y축의 길이
        YAxisBox.Center += YAxisBox.AxisY * JointAxisBox.ExtentY; // Y축 방향으로 이동
        OrientedBoxes.Add(TPair<Shape::FOrientedBox, FLinearColor>(YAxisBox, FLinearColor(0, 1, 0, 0.5))); // 초록색

        Shape::FOrientedBox ZAxisBox = JointAxisBox;
        ZAxisBox.ExtentZ = AxisBoxLength; // Z축의 길이
        ZAxisBox.Center += ZAxisBox.AxisZ * JointAxisBox.ExtentZ; // Z축 방향으로 이동
        OrientedBoxes.Add(TPair<Shape::FOrientedBox, FLinearColor>(ZAxisBox, FLinearColor(0, 0, 1, 0.5))); // 파란색
    }

    // Rigid Body
    // 선택된 Bone은 다른색으로

    for (UBodySetup* BodySetup : PhysicsAsset->BodySetup)
    {
        FName BoneName = BodySetup->BoneName;
        int32 BoneIndex = ReferenceSkeleton->FindBoneIndex(BoneName);
        if (BoneIndex == INDEX_NONE)
        {
            continue; // 해당 Bone이 없으면 건너뜀
        }

        FLinearColor Color = (TargetBodySetup == BodySetup) ? FLinearColor(0, 1, 1, 0.5) : FLinearColor(1, 0, 1, 0.3);
        if (!bPreviewWorld)
        {
            if (UEditorEngine* Engine = Cast<UEditorEngine>(GEngine))
            {
                if (Engine->GetSelectedActor() == SkelComp->GetOwner())
                {
                    Color = FLinearColor(0, 1, 1, 0.3);
                }
            }
        }

        FMatrix InitialMatrix = SkelComp->GetBoneComponentSpaceTransform(BoneIndex).ToMatrixNoScale()
            * SkelComp->GetWorldMatrix();
        InitialMatrix.GetMatrixWithoutScale();

        FVector InitialPosition = InitialMatrix.GetTranslationVector();
        FQuat InitialRotation = InitialMatrix.ToQuat();
        InitialRotation.Normalize();

        FTransform InitialTransform(InitialRotation, InitialPosition);

        FKAggregateGeom AggGeom = BodySetup->AggGeom;

        for (FKSphereElem& SphereElem : AggGeom.SphereElems)
        {
            FTransform Src = FTransform(SphereElem.Center);
            FTransform Dst;
            Dst = InitialTransform * Src;

            SphereElem.Center = Dst.GetTranslation();
            Shape::FSphere Sphere(SphereElem.Center, SphereElem.Radius);
            Spheres.Add(TPair<Shape::FSphere, FLinearColor>(Sphere, Color));
        }

        for (FKBoxElem& BoxElem : AggGeom.BoxElems)
        {
            FQuat Rotation = BoxElem.Rotation.Quaternion();
            FTransform Src = { Rotation, BoxElem.Center };
            FTransform Dst;
            Dst = InitialTransform * Src;

            BoxElem.SetTransform(Dst);
            Shape::FOrientedBox OrientedBox = BoxElem.ToFOrientedBox();
            OrientedBoxes.Add(TPair<Shape::FOrientedBox, FLinearColor>(OrientedBox, Color));
        }

        for (FKSphylElem& SphylElem : AggGeom.SphylElems)
        {
            FQuat Rotation = SphylElem.Rotation.Quaternion();
            FTransform Src = { Rotation, SphylElem.Center };
            FTransform Dst;
            Dst = InitialTransform * Src;

            SphylElem.SetTransform(Dst);
            Shape::FCapsule Capsule = SphylElem.ToFCapsule();
            Capsules.Add(TPair<Shape::FCapsule, FLinearColor>(Capsule, Color));
        }
    }

    // Constraint
    // 선택된 Constraint은 다른색으로
    // 월드 에디터에서는 나오지 않음
    if (!bPreviewWorld)
    {
        return;
    }
    TArray<UPhysicsConstraintTemplate*>& ConstraintSetup = PhysicsAsset->ConstraintSetup;
    for (const UPhysicsConstraintTemplate* Template : ConstraintSetup)
    {
        if (Template == nullptr)
        {
            continue; // 유효하지 않은 템플릿은 건너뜀
        }

        const FConstraintInstance& Instance = Template->DefaultInstance;

        FName JointName = Instance.JointName;
        int32 BoneIndex = ReferenceSkeleton->FindBoneIndex(JointName);
        if (BoneIndex == INDEX_NONE)
        {
            continue; // 해당 Bone이 없으면 건너뜀
        }
        float Radius = (BoneIndex == SelectedConstraintIndex) ? 15 : 5;

        FMatrix InitialMatrix = SkelComp->GetBoneComponentSpaceTransform(BoneIndex).ToMatrixNoScale()
            * SkelComp->GetWorldMatrix();
        InitialMatrix.GetMatrixWithoutScale();

        FMatrix CorrectionMatrix;
        CorrectionMatrix.M[0][0] = 0.f; CorrectionMatrix.M[0][1] = 0.f; CorrectionMatrix.M[0][2] = 1.f; CorrectionMatrix.M[0][3] = 0.f;
        CorrectionMatrix.M[1][0] = 1.f; CorrectionMatrix.M[1][1] = 0.f; CorrectionMatrix.M[1][2] = 0.f; CorrectionMatrix.M[1][3] = 0.f;
        CorrectionMatrix.M[2][0] = 0.f; CorrectionMatrix.M[2][1] = 1.f; CorrectionMatrix.M[2][2] = 0.f; CorrectionMatrix.M[2][3] = 0.f;
        CorrectionMatrix.M[3][0] = 0.f; CorrectionMatrix.M[3][1] = 0.f; CorrectionMatrix.M[3][2] = 0.f; CorrectionMatrix.M[3][3] = 1.f;
        FQuat CorrectionRotation = FQuat(CorrectionMatrix);
        CorrectionRotation.Normalize();

        FVector InitialPosition = InitialMatrix.GetTranslationVector();
        FQuat InitialRotation = InitialMatrix.ToQuat() * CorrectionRotation;
        InitialRotation.Normalize();

        FTransform InitialTransform(InitialRotation, InitialPosition);

        // Joint의 위치와 방향을 표시하기 위해 Cone을 사용
        const FConstraintProfileProperties& Profile = Instance.ProfileInstance;
        //const FLinearConstraint& Linear = Profile.LinearLimit;

        const FConeConstraint& Cone = Profile.ConeLimit;
        Shape::FEllipticalCone ConeShape;
        ConeShape.ApexPosition = InitialPosition;
        ConeShape.Direction = InitialRotation.RotateVector(FVector(1, 0, 0));
        ConeShape.AngleWidth = Cone.Swing1LimitDegrees;
        ConeShape.AngleHeight = Cone.Swing2LimitDegrees;
        ConeShape.AngleWidth = FMath::DegreesToRadians(ConeShape.AngleWidth);
        ConeShape.AngleHeight = FMath::DegreesToRadians(ConeShape.AngleHeight);
        ConeShape.Radius = Radius;
        EllipticalCones.Add(TPair<Shape::FEllipticalCone, FLinearColor>(ConeShape, FLinearColor(1, 0, 0, 0.2)));

        // Twist Limit
        // Twist는 X축을 기준으로 회전하는 것이기 때문에, Visualize용 Cone의 방향은 X축에 수직이기만 하면 됨
        // 언리얼에서는 Y축을 기준으로 시작하고, 두 배의 각도를 사용함 (180도이면 360도짜리 Cone이 됨)
        const FTwistConstraint& Twist = Profile.TwistLimit;
        Shape::FEllipticalCone TwistShape;
        TwistShape.ApexPosition = InitialPosition;
        TwistShape.Direction = InitialRotation.RotateVector(FVector(0, 1, 0));
        TwistShape.AngleHeight = Twist.TwistLimitDegrees;
        TwistShape.AngleWidth = 0; // 높이는 0으로 설정
        TwistShape.AngleHeight = FMath::DegreesToRadians(TwistShape.AngleHeight);
        TwistShape.Radius = Radius;
        EllipticalCones.Add(TPair<Shape::FEllipticalCone, FLinearColor>(TwistShape, FLinearColor(0, 1, 0, 0.2)));
    }
}
