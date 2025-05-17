#include "ParticleSystemComponent.h"

UFXSystemComponent::UFXSystemComponent()
{
}

UParticleSystemComponent::UParticleSystemComponent()
{
}
TArray<FDynamicEmitterDataBase*>& UParticleSystemComponent::GetEmitterRenderData()
{
    return EmitterRenderData;
}
