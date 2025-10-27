#pragma once
#include "Classes/GameFramework/Actor.h"
#include "Classes/Components/BoxComponent.h"
#include "Classes/Components/StaticMeshComponent.h"

class AGoalPlatformActor : public AActor
{
    DECLARE_CLASS(AGoalPlatformActor, AActor)

public:
    AGoalPlatformActor();
    virtual ~AGoalPlatformActor() override = default;

protected:
    UPROPERTY
    (UBoxComponent*, BoxComponent, = nullptr)

    UPROPERTY
    (UStaticMeshComponent*, MeshComponent, = nullptr)
};
