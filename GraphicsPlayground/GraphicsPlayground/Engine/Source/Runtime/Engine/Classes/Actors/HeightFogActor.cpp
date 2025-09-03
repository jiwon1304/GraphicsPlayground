#include "HeightFogActor.h"

#include "Engine/Classes/Components/HeightFogComponent.h"

AHeightFogActor::AHeightFogActor()
{
    HeightFogComponent = AddComponent<UHeightFogComponent>("UHeightFogComponent_0");

    
}
