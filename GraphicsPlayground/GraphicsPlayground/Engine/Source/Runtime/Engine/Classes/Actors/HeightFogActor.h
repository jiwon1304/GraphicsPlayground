#pragma once
#include "Engine/Classes/GameFramework/Actor.h"

class UHeightFogComponent;


class AHeightFogActor : public AActor
{
    DECLARE_CLASS(AHeightFogActor, AActor)

public:
    AHeightFogActor();

    UPROPERTY_WITH_FLAGS(
        VisibleAnywhere,
        UHeightFogComponent*, HeightFogComponent
    )
};
