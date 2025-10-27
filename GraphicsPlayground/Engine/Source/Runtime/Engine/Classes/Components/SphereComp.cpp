#include "SphereComp.h"
#include "Engine/Source/Runtime/Core/Math/JungleMath.h"
#include "Editor/UnrealEd/EditorViewportClient.h"

USphereComp::USphereComp()
{
    SetType(StaticClass()->GetName());
    AABB.MaxLocation = {1, 1, 1};
    AABB.MinLocation = {-1, -1, -1};
}

void USphereComp::InitializeComponent()
{
    Super::InitializeComponent();
}

void USphereComp::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}