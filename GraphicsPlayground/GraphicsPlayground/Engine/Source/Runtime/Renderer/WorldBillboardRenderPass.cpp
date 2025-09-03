
#include "WorldBillboardRenderPass.h"

#include "UnrealClient.h"
#include "Engine/Classes/Engine/Engine.h"
#include "CoreUObject/UObject/UObjectIterator.h"
#include "Engine/Classes/Components/BillboardComponent.h"

FWorldBillboardRenderPass::FWorldBillboardRenderPass()
{
    ResourceType = EResourceType::ERT_Scene;
}

void FWorldBillboardRenderPass::PrepareRenderArr()
{
    BillboardComps.Empty();
    for (const auto Component : TObjectRange<UBillboardComponent>())
    {
        if (Component->GetWorld() == GEngine->ActiveWorld && !Component->bIsEditorBillboard)
        {
            BillboardComps.Add(Component);
        }
    }
}
