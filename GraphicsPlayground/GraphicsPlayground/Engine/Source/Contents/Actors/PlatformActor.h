#pragma once
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"


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
