#include "HeightFogActor.h"

#include "Classes/Components/HeightFogComponent.h"

AHeightFogActor::AHeightFogActor()
{
    HeightFogComponent = AddComponent<UHeightFogComponent>("UHeightFogComponent_0");

    
}
