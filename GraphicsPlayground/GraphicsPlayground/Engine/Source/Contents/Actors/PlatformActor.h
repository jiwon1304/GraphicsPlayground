#pragma once
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Classes/Components/BoxComponent.h"
#include "Classes/Components/StaticMeshComponent.h"


class APlatformActor : public AActor
{
    DECLARE_CLASS(APlatformActor, AActor)

public:
    APlatformActor();
    virtual ~APlatformActor() override = default;

protected:
    UPROPERTY(
        VisibleAnywhere,
        UBoxComponent*, BoxComponent, = nullptr;
    )

    UPROPERTY(
        VisibleAnywhere,
        UStaticMeshComponent*, MeshComponent, = nullptr;
    )
};
