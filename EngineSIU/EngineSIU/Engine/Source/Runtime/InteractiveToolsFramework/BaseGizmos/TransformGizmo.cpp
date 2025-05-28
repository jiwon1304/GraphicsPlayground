#include "TransformGizmo.h"
#include "GizmoArrowComponent.h"
#include "Define.h"
#include "GizmoCircleComponent.h"
#include "Actors/Player.h"
#include "GizmoRectangleComponent.h"
#include "ReferenceSkeleton.h"
#include "Animation/Skeleton.h"
#include "Engine/EditorEngine.h"
#include "World/World.h"
#include "Engine/FObjLoader.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "World/PhysicsAssetWorld.h"

ATransformGizmo::ATransformGizmo()
{
    static int a = 0;
    UE_LOG(ELogLevel::Error, "Gizmo Created %d", a++);
    FObjManager::CreateStaticMesh("Assets/GizmoTranslationX.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoTranslationY.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoTranslationZ.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoRotationX.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoRotationY.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoRotationZ.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoScaleX.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoScaleY.obj");
    FObjManager::CreateStaticMesh("Assets/GizmoScaleZ.obj");

    SetRootComponent(
        AddComponent<USceneComponent>()
    );

    UGizmoArrowComponent* LocationX = AddComponent<UGizmoArrowComponent>();
    LocationX->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoTranslationX.obj"));
    LocationX->SetupAttachment(RootComponent);
    LocationX->SetGizmoType(UGizmoBaseComponent::ArrowX);
    ArrowArr.Add(LocationX);

    UGizmoArrowComponent* LocationY = AddComponent<UGizmoArrowComponent>();
    LocationY->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoTranslationY.obj"));
    LocationY->SetupAttachment(RootComponent);
    LocationY->SetGizmoType(UGizmoBaseComponent::ArrowY);
    ArrowArr.Add(LocationY);

    UGizmoArrowComponent* LocationZ = AddComponent<UGizmoArrowComponent>();
    LocationZ->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoTranslationZ.obj"));
    LocationZ->SetupAttachment(RootComponent);
    LocationZ->SetGizmoType(UGizmoBaseComponent::ArrowZ);
    ArrowArr.Add(LocationZ);

    UGizmoRectangleComponent* ScaleX = AddComponent<UGizmoRectangleComponent>();
    ScaleX->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoScaleX.obj"));
    ScaleX->SetupAttachment(RootComponent);
    ScaleX->SetGizmoType(UGizmoBaseComponent::ScaleX);
    RectangleArr.Add(ScaleX);

    UGizmoRectangleComponent* ScaleY = AddComponent<UGizmoRectangleComponent>();
    ScaleY->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoScaleY.obj"));
    ScaleY->SetupAttachment(RootComponent);
    ScaleY->SetGizmoType(UGizmoBaseComponent::ScaleY);
    RectangleArr.Add(ScaleY);

    UGizmoRectangleComponent* ScaleZ = AddComponent<UGizmoRectangleComponent>();
    ScaleZ->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoScaleZ.obj"));
    ScaleZ->SetupAttachment(RootComponent);
    ScaleZ->SetGizmoType(UGizmoBaseComponent::ScaleZ);
    RectangleArr.Add(ScaleZ);

    UGizmoCircleComponent* CircleX = AddComponent<UGizmoCircleComponent>();
    CircleX->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoRotationX.obj"));
    CircleX->SetupAttachment(RootComponent);
    CircleX->SetGizmoType(UGizmoBaseComponent::CircleX);
    CircleArr.Add(CircleX);

    UGizmoCircleComponent* CircleY = AddComponent<UGizmoCircleComponent>();
    CircleY->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoRotationY.obj"));
    CircleY->SetupAttachment(RootComponent);
    CircleY->SetGizmoType(UGizmoBaseComponent::CircleY);
    CircleArr.Add(CircleY);

    UGizmoCircleComponent* CircleZ = AddComponent<UGizmoCircleComponent>();
    CircleZ->SetStaticMesh(FObjManager::GetStaticMesh(L"Assets/GizmoRotationZ.obj"));
    CircleZ->SetupAttachment(RootComponent);
    CircleZ->SetGizmoType(UGizmoBaseComponent::CircleZ);
    CircleArr.Add(CircleZ);
}

void ATransformGizmo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Editor 모드에서만 Tick. SkeletalMeshViewer모드에서도 tick
    if (GEngine->ActiveWorld->WorldType != EWorldType::Editor and GEngine->ActiveWorld->WorldType != EWorldType::SkeletalViewer && GEngine->ActiveWorld->WorldType != EWorldType::PhysicsAssetEditor)
    {
        return;
    }

    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return;
    }
    AEditorPlayer* EditorPlayer = Engine->GetEditorPlayer();
    if (!EditorPlayer)
    {
        return;
    }
    
    USceneComponent* SelectedComponent = Engine->GetSelectedComponent();
    AActor* SelectedActor = Engine->GetSelectedActor();

    USceneComponent* TargetComponent = nullptr;

    if (SelectedComponent != nullptr)
    {
        TargetComponent = SelectedComponent;
    }
    else if (SelectedActor != nullptr)
    {
        TargetComponent = SelectedActor->GetRootComponent();
    }

    if (TargetComponent)
    {
        SetActorLocation(TargetComponent->GetComponentLocation());
        if (EditorPlayer->GetCoordMode() == ECoordMode::CDM_LOCAL || EditorPlayer->GetControlMode() == EControlMode::CM_SCALE)
        {
            SetActorRotation(TargetComponent->GetComponentRotation());
        }
        else
        {
            SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
        }

        //본 부착용
        if (GEngine->ActiveWorld->WorldType == EWorldType::SkeletalViewer || GEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
        {
            int32 BoneIndex = -1;
            if (GEngine->ActiveWorld->WorldType == EWorldType::SkeletalViewer)
            {
                BoneIndex = Engine->SkeletalMeshViewerWorld->SelectBoneIndex;
            }
            else if (GEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
            {
                int32 SelectedBoneIndex = Engine->PhysicsAssetEditorWorld->SelectBoneIndex;
                int32 SelectedBodySetupIndex = Engine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
                int32 SelectedConstraintIndex = Engine->PhysicsAssetEditorWorld->SelectedConstraintIndex;
                
                int32 ParentBodySetupIndex = Engine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex;
                int32 SelectedPrimitiveIndex = Engine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex;
                EAggCollisionShape::Type PrimitiveType = Engine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType;

                if (SelectedBoneIndex != -1)
                {
                    BoneIndex = SelectedBoneIndex;
                }
                else if (SelectedBodySetupIndex != -1)
                {
                    USkeletalMesh* SkeletalMesh = Engine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
                    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
                    UBodySetup* BodySetup = PhysicsAsset->BodySetup[SelectedBodySetupIndex];
                    BoneIndex = SkeletalMesh->GetRefSkeleton()->FindBoneIndex(BodySetup->BoneName);
                }
                else if (SelectedConstraintIndex != -1)
                {
                    USkeletalMesh* SkeletalMesh = Engine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
                    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
                    BoneIndex = SkeletalMesh->GetRefSkeleton()->FindBoneIndex(PhysicsAsset->ConstraintSetup[SelectedConstraintIndex]->DefaultInstance.ConstraintBone1);
                }
                else if (ParentBodySetupIndex != -1 && SelectedPrimitiveIndex != -1 && PrimitiveType != EAggCollisionShape::Unknown)
                {
                    USkeletalMesh* SkeletalMesh = Engine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
                    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();
                    UBodySetup* BodySetup = PhysicsAsset->BodySetup[ParentBodySetupIndex];
                    BoneIndex = SkeletalMesh->GetRefSkeleton()->FindBoneIndex(BodySetup->BoneName);
                }
            }

            if (BoneIndex != -1)
            {
                USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(TargetComponent);
                if (SkeletalMeshComp)
                {
                    TArray<FMatrix> GlobalBoneMatrices;
                    SkeletalMeshComp->GetCurrentGlobalBoneMatrices(GlobalBoneMatrices);

                    FTransform GlobalBoneTransform = FTransform(GlobalBoneMatrices[BoneIndex]);

                    AddActorLocation(GlobalBoneTransform.Translation);
                    if (EditorPlayer->GetCoordMode() == ECoordMode::CDM_LOCAL || EditorPlayer->GetControlMode() == EControlMode::CM_SCALE || GEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
                    {
                        AddActorRotation(GlobalBoneTransform.Rotation);
                    }
                }
            }
        }


        if (GEngine->ActiveWorld->WorldType == EWorldType::PhysicsAssetEditor)
        {
            int BodySetupIndex = Engine->PhysicsAssetEditorWorld->SelectedBodySetupIndex;
            int PrimitiveIndex = Engine->PhysicsAssetEditorWorld->SelectedPrimitive.SelectedPrimitiveIndex;
            int ParentBodySetupIndex = Engine->PhysicsAssetEditorWorld->SelectedPrimitive.ParentBodySetupIndex;
            EAggCollisionShape::Type PrimitiveType = Engine->PhysicsAssetEditorWorld->SelectedPrimitive.PrimitiveType;

            if (BodySetupIndex == -1 && (PrimitiveIndex == -1 || ParentBodySetupIndex == -1 || PrimitiveType == EAggCollisionShape::Unknown))
            {
                return;
            }

            USkeletalMesh* SkeletalMesh = Engine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
            UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();

            UBodySetup* TargetBodySetup;
            if (BodySetupIndex != -1)
            {
                TargetBodySetup = PhysicsAsset->BodySetup[BodySetupIndex];
            }
            else
            {
                TargetBodySetup = PhysicsAsset->BodySetup[ParentBodySetupIndex];
            }

            if (TargetBodySetup == nullptr || (TargetBodySetup->AggGeom.BoxElems.Num() == 0 && TargetBodySetup->AggGeom.SphereElems.Num() == 0 && TargetBodySetup->AggGeom.SphylElems.Num() == 0))
            {
                return;
            }

            FKShapeElem* TargetAggregateGeom = nullptr;
            EAggCollisionShape::Type TargetPrimitiveType = EAggCollisionShape::Unknown;
            if (PrimitiveType != EAggCollisionShape::Unknown && PrimitiveIndex != -1)
            {
                TargetPrimitiveType = PrimitiveType;
                if (PrimitiveType == EAggCollisionShape::Sphere)
                {
                    TargetAggregateGeom = &(TargetBodySetup->AggGeom.SphereElems[PrimitiveIndex]);
                }
                else if (PrimitiveType == EAggCollisionShape::Box)
                {
                    TargetAggregateGeom = &(TargetBodySetup->AggGeom.BoxElems[PrimitiveIndex]);
                }
                else if (PrimitiveType == EAggCollisionShape::Sphyl)
                {
                    TargetAggregateGeom = &(TargetBodySetup->AggGeom.SphylElems[PrimitiveIndex]);
                }
            }
            else
            {
                if (TargetBodySetup->AggGeom.SphereElems.Num() > 0)
                {
                    TargetPrimitiveType = EAggCollisionShape::Sphere;
                    TargetAggregateGeom = &(TargetBodySetup->AggGeom.SphereElems[0]);
                }
                else if (TargetBodySetup->AggGeom.BoxElems.Num() > 0)
                {
                    TargetPrimitiveType = EAggCollisionShape::Box;
                    TargetAggregateGeom = &(TargetBodySetup->AggGeom.BoxElems[0]);
                }
                else if (TargetBodySetup->AggGeom.SphylElems.Num() > 0)
                {
                    TargetPrimitiveType = EAggCollisionShape::Sphyl;
                    TargetAggregateGeom = &(TargetBodySetup->AggGeom.SphylElems[0]);
                }
            }
            
            if (TargetAggregateGeom != nullptr && TargetPrimitiveType != EAggCollisionShape::Unknown)
            {
                TArray<FMatrix> GlobalBoneMatrices;
                Engine->PhysicsAssetEditorWorld->GetSkeletalMeshComponent()->GetCurrentGlobalBoneMatrices(GlobalBoneMatrices);
                int32 BoneIndex = SkeletalMesh->GetRefSkeleton()->FindBoneIndex(TargetBodySetup->BoneName);
                FTransform GlobalBoneTransform = FTransform(GlobalBoneMatrices[BoneIndex]);
                
                if (TargetPrimitiveType == EAggCollisionShape::Sphere)
                {
                    FKSphereElem SphereElem = *static_cast<FKSphereElem*>(TargetAggregateGeom);
                    FVector NewLocation = GetRootComponent()->GetComponentTransform().TransformDirection(SphereElem.Center);
                    AddActorLocation(NewLocation);
                }
                else if (TargetPrimitiveType == EAggCollisionShape::Box)
                {
                    FKBoxElem BoxElem = *static_cast<FKBoxElem*>(TargetAggregateGeom);
                    FVector NewLocation = GetRootComponent()->GetComponentTransform().TransformDirection(BoxElem.Center);
                    AddActorLocation(NewLocation);
                    if (EditorPlayer->GetCoordMode() == ECoordMode::CDM_LOCAL || EditorPlayer->GetControlMode() == EControlMode::CM_SCALE)
                    {
                        AddActorRotation(BoxElem.Rotation);
                    }
                }
                else if (TargetPrimitiveType == EAggCollisionShape::Sphyl)
                {
                    FKSphylElem SphylElem = *static_cast<FKSphylElem*>(TargetAggregateGeom);
                    FVector NewLocation = GetRootComponent()->GetComponentTransform().TransformDirection(SphylElem.Center);
                    AddActorLocation(NewLocation);
                    if (EditorPlayer->GetCoordMode() == ECoordMode::CDM_LOCAL || EditorPlayer->GetControlMode() == EControlMode::CM_SCALE)
                    {
                        AddActorRotation(SphylElem.Rotation);
                    }
                }
            }
        }
    }


    //

}

void ATransformGizmo::Initialize(FEditorViewportClient* InViewport)
{
    AttachedViewport = InViewport;
}
