#include "ParticleSystem.h"
#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Components/ParticleSystemComponent.h"
#include "CoreUObject/UObject/Casts.h"

void UParticleSystem::PostEditChangeProperty()
{
    if (!PreviewComponent)
    {
        PreviewComponent = Cast<UParticleSystemComponent>(GetOuter());
        if (!PreviewComponent)
        {
            UE_LOG(ELogLevel::Error, TEXT("PreviewComponent is Null!!!"));
            return;
        }
    }

    BuildEmitters();
    PreviewComponent->UpdateInstances();
}

void UParticleSystem::BuildEmitters()
{
    const int32 EmitterCount = Emitters.Num();
    for (int32 EmitterIndex = 0; EmitterIndex < EmitterCount; ++EmitterIndex)
    {
        if (UParticleEmitter* Emitter = Emitters[EmitterIndex])
        {
            Emitter->UpdateModuleLists();
        }
    }
}
