#include "CapsuleActor.h"
#include "Engine/Classes/Components/CapsuleComponent.h"

ACapsuleActor::ACapsuleActor()
{
    CapsuleComponent = AddComponent<UCapsuleComponent>();
    RootComponent = CapsuleComponent;
}

UCapsuleComponent* ACapsuleActor::GetShapeComponent() const
{
    return CapsuleComponent;
}
