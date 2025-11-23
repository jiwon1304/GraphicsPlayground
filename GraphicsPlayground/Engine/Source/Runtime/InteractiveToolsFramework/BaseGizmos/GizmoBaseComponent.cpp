#include "GizmoBaseComponent.h"

#include "TransformGizmo.h"
#include "Classes/GameFramework/Actor.h"
#include "Editor/LevelEditor/SLevelEditor.h"
#include "Editor/UnrealEd/EditorViewportClient.h"


void UGizmoBaseComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    if (!GetOwner())
    {
        return;
    }
    
    if (FEditorViewportClient* ViewportClient = Cast<ATransformGizmo>(GetOwner())->GetAttachedViewport())
    {
        if (ViewportClient->IsPerspective())
        {
            float Scaler = (ViewportClient->GetViewportCamera()->ViewLocation- GetOwner()->GetActorLocation()).Length();
            
            Scaler *= GizmoScale;
            RelativeScale3D = FVector(Scaler);
        }
        else
        {
            float Scaler = FEditorViewportClient::OrthoSize * GizmoScale;
            RelativeScale3D = FVector(Scaler);
        }
    }
}
