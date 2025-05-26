#include "PhysicsAssetWorld.h"

#include "Engine/EditorEngine.h"

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

void UPhysicsAssetWorld::ClearSelected()
{
    SelectBoneIndex = -1;
    SelectedBodySetupIndex = -1;
    SelectedPrimitive = FSelectedPrimitive();
}
