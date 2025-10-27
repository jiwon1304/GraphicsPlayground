
#include "EditorBillboardRenderPass.h"

#include "Engine/UnrealClient.h"
#include "Classes/Engine/Engine.h"
#include "CoreUObject/UObject/UObjectIterator.h"
#include "Classes/Components/BillboardComponent.h"

FEditorBillboardRenderPass::FEditorBillboardRenderPass()
{
    ResourceType = EResourceType::ERT_Editor;
}

void FEditorBillboardRenderPass::PrepareRenderArr()
{
    BillboardComps.Empty();
    for (const auto Component : TObjectRange<UBillboardComponent>())
    {
        if (Component->GetWorld() == GEngine->ActiveWorld && Component->bIsEditorBillboard)
        {
            BillboardComps.Add(Component);
        }
    }
}
