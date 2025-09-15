#include "CubeActor.h"
#include "Classes/Components/BoxComponent.h"

ACubeActor::ACubeActor()
{
    BoxComponent = AddComponent<UBoxComponent>();
    RootComponent = BoxComponent;
}

UBoxComponent* ACubeActor::GetShapeComponent() const
{
    return BoxComponent;
}
