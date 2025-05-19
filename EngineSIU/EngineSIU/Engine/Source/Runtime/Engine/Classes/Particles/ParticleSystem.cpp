#include "ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Components/ParticleSystemComponent.h"

void UParticleSystem::PostEditChangeProperty()
{
    if (!PreviewComponent)
    {
        UE_LOG(ELogLevel::Error, TEXT("PreviewComponent is Null!!!"));
        return;
    }

    PreviewComponent->UpdateInstances();
}

void UParticleSystem::BuildEmitters()
{
    const int32 EmitterCount = Emitters.Num();
    for (int32 EmitterIndex = 0; EmitterIndex < EmitterCount; ++EmitterIndex)
    {
        if (UParticleEmitter* Emitter = Emitters[EmitterIndex])
        {
            Emitter->Build();
        }
    }
}
