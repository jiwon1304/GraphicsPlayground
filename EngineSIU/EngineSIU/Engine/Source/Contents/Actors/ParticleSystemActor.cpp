#include "ParticleSystemActor.h"
#include "Particles/ParticleSystemComponent.h"

AParticleSystemActor::AParticleSystemActor()
{
    ParticleSystemComponent = AddComponent<UParticleSystemComponent>("ParticleSystemComponent_0");
    RootComponent = ParticleSystemComponent;

}

UParticleSystemComponent* AParticleSystemActor::GetParticleSystemComponent() const
{
    return ParticleSystemComponent;
}
