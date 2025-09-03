#pragma once
#include "Engine/Classes/GameFramework/Actor.h"

class UParticleSystemComponent;

class AParticleSystemActor : public AActor
{
    DECLARE_CLASS(AParticleSystemActor, AActor)

public:
    AParticleSystemActor();

    UParticleSystemComponent* GetParticleSystemComponent() const;

protected:
    UPROPERTY(UParticleSystemComponent*, ParticleSystemComponent, = nullptr);
};
 
