
#include "WorldBillboardRenderPass.h"

#include "Engine/UnrealClient.h"
#include "Classes/Engine/Engine.h"
#include "CoreUObject/UObject/UObjectIterator.h"
#include "Classes/Components/BillboardComponent.h"

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
