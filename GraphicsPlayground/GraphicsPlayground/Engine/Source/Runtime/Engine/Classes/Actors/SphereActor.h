#pragma once
#include "Engine/Classes/GameFramework/Actor.h"

class USphereComponent;


class ASphereActor : public AActor
{
    DECLARE_CLASS(ASphereActor, AActor)
public:
    ASphereActor();

    USphereComponent* GetShapeComponent() const;
protected:
    UPROPERTY(
        VisibleAnywhere,
        USphereComponent*, SphereComponent, = nullptr;
    )
};

