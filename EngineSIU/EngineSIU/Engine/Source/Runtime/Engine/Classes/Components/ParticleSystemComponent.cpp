#include "ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleEmitterInstance.h"
#include "Particles/ParticleLODLevel.h"

void UParticleSystemComponent::InitializeSystem()
{
    assert(Template);
    InitParticles();
}

void UParticleSystemComponent::InitParticles()
{
    int32 NumInstances = EmitterInstances.Num();
    int32 NumEmitters = Template->Emitters.Num();
    const bool bIsFirstCreate = (NumInstances == 0);

    bWasCompleted = bIsFirstCreate ? false : bWasCompleted;

    int32 PreferredLODLevel = LODLevel;

    for (int32 Idx = 0; Idx < NumEmitters; Idx++)
    {
        UParticleEmitter* Emitter = Template->Emitters[Idx];
        if (Emitter)
        {
            FParticleEmitterInstance* Instance = NumInstances == 0 ? nullptr : EmitterInstances[Idx];
            if (!Instance)
            {
                Instance = Emitter->CreateInstance(this);
                EmitterInstances[Idx] = Instance;
            }
            Instance->bEnabled = true;
            Instance->InitParameters(Emitter, this);
            Instance->Init();

            Instance->CurrentLODLevelIndex = LODLevel;
            Instance->CurrentLODLevel = Instance->SpriteTemplate->LODLevels[Instance->CurrentLODLevelIndex];
        }
    }
}

void UParticleSystemComponent::TickComponent(float DeltaTime)
{
    for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
    {
        FParticleEmitterInstance* Instance = EmitterInstances[EmitterIndex];
        if (Instance && Instance->SpriteTemplate)
        {
            UParticleLODLevel* SpriteLODLevel = Instance->SpriteTemplate->GetCurrentLODLevel(Instance);
            if (SpriteLODLevel && SpriteLODLevel->bEnabled)
            {
                Instance->Tick(DeltaTime);
            }
        }
    }
}

void UParticleSystemComponent::SetTemplate(UParticleSystem* InTemplate)
{
    if (InTemplate != Template)
    {
        Template = InTemplate;
        EmitterInstances.Empty();
        InitializeSystem();
    }
}
