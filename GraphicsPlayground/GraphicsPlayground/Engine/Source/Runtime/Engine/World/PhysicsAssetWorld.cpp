#include "PhysicsAssetWorld.h"

#include "Engine/EditorEngine.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"

UPhysicsAssetWorld* UPhysicsAssetWorld::CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName)
{
    UPhysicsAssetWorld* NewWorld = FObjectFactory::ConstructObject<UPhysicsAssetWorld>(InOuter);
    NewWorld->WorldName = InWorldName;
    NewWorld->WorldType = InWorldType;
    NewWorld->InitializeNewWorld();
    
    return NewWorld;
}

void UPhysicsAssetWorld::Tick(float DeltaTime)
{
    // TODO UISOO Check (늦어도 ㄱㅊ)
    UWorld::Tick(DeltaTime);

    //TODO: 임시로 SkeletalMeshComponent을 강제로 셀렉트 함
    Cast<UEditorEngine>(GEngine)->SelectActor(SkeletalMeshComponent->GetOwner());
    Cast<UEditorEngine>(GEngine)->SelectComponent(SkeletalMeshComponent);
}

FTransform UPhysicsAssetWorld::GetSelectedTransform()
{
    FTransform Result = FTransform::Identity;
    FKShapeElem* TargetAggregateGeom = nullptr;
    UBodySetup* TargetBodySetup = nullptr;
    EAggCollisionShape::Type TargetPrimitiveType = EAggCollisionShape::Unknown;
    
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return Result;
    }
    AEditorPlayer* EditorPlayer = Engine->GetEditorPlayer();
    if (!EditorPlayer)
    {
        return Result;
    }

    USkeletalMeshComponent* SkeletalMeshComp = GetSkeletalMeshComponent();

    if (SkeletalMeshComp == nullptr)
    {
        return Result;
    }
    Result = Result * SkeletalMeshComp->GetComponentTransform();

    int BoneIndex = SelectBoneIndex;

    if (BoneIndex != -1)
    {
        Result = GetSkeletalMeshComponent()->GetBoneComponentSpaceTransform(BoneIndex) * Result;
    }

    
    int BodySetupIndex = SelectedBodySetupIndex;
    int ConstraintIndex = SelectedConstraintIndex;
    int PrimitiveIndex = SelectedPrimitive.SelectedPrimitiveIndex;
    int ParentBodySetupIndex = SelectedPrimitive.ParentBodySetupIndex;
    EAggCollisionShape::Type PrimitiveType = SelectedPrimitive.PrimitiveType;

    if (BodySetupIndex == -1 && ConstraintIndex == -1 && (PrimitiveIndex == -1 || ParentBodySetupIndex == -1 || PrimitiveType == EAggCollisionShape::Unknown))
    {
        return Result;
    }

    USkeletalMesh* SkeletalMesh = GetSkeletalMeshComponent()->GetSkeletalMeshAsset();
    UPhysicsAsset* PhysicsAsset = SkeletalMesh->GetPhysicsAsset();

    if (BodySetupIndex != -1)
    {
        TargetBodySetup = PhysicsAsset->BodySetup[BodySetupIndex];
    }
    else if (ConstraintIndex != -1)
    {
        int32 TempBodyIndex = PhysicsAsset->FindBodyIndex(PhysicsAsset->ConstraintSetup[ConstraintIndex]->DefaultInstance.ConstraintBone1);
        if (TempBodyIndex == -1)
        {
            return Result;
        }
        TargetBodySetup = PhysicsAsset->BodySetup[TempBodyIndex];
    }
    else
    {
        TargetBodySetup = PhysicsAsset->BodySetup[ParentBodySetupIndex];
    }

    if (TargetBodySetup == nullptr || (TargetBodySetup->AggGeom.BoxElems.Num() == 0 && TargetBodySetup->AggGeom.SphereElems.Num() == 0 && TargetBodySetup->AggGeom.SphylElems.Num() == 0))
    {
        return Result;
    }

    if (BoneIndex == -1)
    {
        BoneIndex = SkeletalMesh->GetRefSkeleton()->FindBoneIndex(TargetBodySetup->BoneName);
        if (BoneIndex == -1)
        {
            return Result;
        }
        Result = GetSkeletalMeshComponent()->GetBoneComponentSpaceTransform(BoneIndex) * Result;
    }
    
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

    if (TargetAggregateGeom == nullptr && TargetPrimitiveType == EAggCollisionShape::Unknown)
    {
        return Result;
    }

    if (TargetPrimitiveType == EAggCollisionShape::Sphere)
    {
        FKSphereElem SphereElem = *static_cast<FKSphereElem*>(TargetAggregateGeom);
        FVector NewLocation = Result.TransformDirection(SphereElem.Center);
        Result.AddToTranslation(NewLocation);
    }
    else if (TargetPrimitiveType == EAggCollisionShape::Box)
    {
        FKBoxElem BoxElem = *static_cast<FKBoxElem*>(TargetAggregateGeom);
        FVector NewLocation = Result.TransformDirection(BoxElem.Center);
        Result.AddToTranslation(NewLocation);
        Result.Rotation = BoxElem.Rotation.Quaternion() * Result.Rotation;
    }
    else if (TargetPrimitiveType == EAggCollisionShape::Sphyl)
    {
        FKSphylElem SphylElem = *static_cast<FKSphylElem*>(TargetAggregateGeom);
        FVector NewLocation = Result.TransformDirection(SphylElem.Center);
        Result.AddToTranslation(NewLocation);
        Result.Rotation = SphylElem.Rotation.Quaternion() * Result.Rotation;
    }

    return Result;
}

void UPhysicsAssetWorld::ClearSelected()
{
    SelectBoneIndex = -1;
    SelectedBodySetupIndex = -1;
    SelectedConstraintIndex = -1;
    SelectedPrimitive = FSelectedPrimitive();
}
