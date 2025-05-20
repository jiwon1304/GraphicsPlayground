#include "ParticleSystemComponent.h"

#include "Material/Material.h"
#include "Particles/ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModules/ParticleModuleRequired.h"

void UParticleSystemComponent::InitializeSystem()
{
    if (!Template)
    {
        UE_LOG(ELogLevel::Error, TEXT("Template ParticleSystem Should be assigned!!!!"));
        return;
    }
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

void UParticleSystemComponent::ResetParticles()
{
    for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
    {
        FParticleEmitterInstance* EmitterInstance = EmitterInstances[EmitterIndex];
        if (EmitterInstance)
        {
            EmitterInstance->SpriteTemplate = nullptr;
            EmitterInstance->Component = nullptr;

            delete EmitterInstance;
            EmitterInstances[EmitterIndex] = nullptr;
        }
    }
    EmitterInstances.Empty();
    ClearDynamicData();
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

void UParticleSystemComponent::UpdateInstances()
{
    ResetParticles();
    InitializeSystem();
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

void UParticleSystemComponent::ClearDynamicData()
{

}
void UParticleSystemComponent::InitTestSpriteParticles()
{
    FParticleSpriteEmitterInstance* Emitter = new FParticleSpriteEmitterInstance();
    Emitter->Component = this;

    Emitter->ParticleStride = sizeof(FBaseParticle);
    Emitter->MaxActiveParticles = 64;
    Emitter->ActiveParticles = 0;

    const int32 ParticleBytes = Emitter->ParticleStride * Emitter->MaxActiveParticles;
    Emitter->ParticleData = new uint8[ParticleBytes];
    std::memset(Emitter->ParticleData, 0, ParticleBytes);

    Emitter->ParticleIndices = new uint16[Emitter->MaxActiveParticles];
    for (int32 i = 0; i < Emitter->MaxActiveParticles; ++i)
        Emitter->ParticleIndices[i] = i;

    EmitterInstances.Add(Emitter);

    const int32 ParticleCount = 20;
    const FVector Positions[ParticleCount] = {
        { 0,  0,  0}, {10,  5,  2}, {20, -4,  3}, {30,  2,  1}, {40, -6,  5},
        { 5, 12,  0}, {15,  9,  3}, {25, -3,  2}, {35,  4,  4}, {45, -5,  6},
        { 0, -8,  1}, {10, -9,  2}, {20,  6,  0}, {30,  7,  3}, {40,  0,  1},
        { 5,  3,  4}, {15, -2,  2}, {25,  8,  5}, {35, -7,  3}, {45,  1,  6},
    };
    const FLinearColor Colors[] = { FLinearColor::Red, FLinearColor::Green, FLinearColor::Blue, FLinearColor::Yellow };

    for (int32 i = 0; i < 20; ++i)
    {
        uint8* Ptr = Emitter->ParticleData + i * Emitter->ParticleStride;
        FBaseParticle* Particle = reinterpret_cast<FBaseParticle*>(Ptr);

        Particle->Location = Positions[i];
        Particle->OldLocation = Positions[i];
        Particle->Size = FVector(5.0f);
        Particle->BaseSize = Particle->Size;
        Particle->Color = Colors[i % 4];
        Particle->BaseColor = Particle->Color;
        Particle->RelativeTime = 0.5f;
        Particle->OneOverMaxLifetime = 1.0f;
    }

    Emitter->ActiveParticles = 20;

    // SpriteTemplate 및 LODLevel 설정 필수
    Emitter->SpriteTemplate = new UParticleEmitter();
    UParticleLODLevel* LOD = new UParticleLODLevel();
    UParticleModuleRequired* RequiredModule = new UParticleModuleRequired();
    RequiredModule->EmitterDuration = 5.0f;
    RequiredModule->EmitterLoops = 0;
    RequiredModule->bUseLocalSpace = false;

    LOD->RequiredModule = RequiredModule;
    LOD->bEnabled = true;
    Emitter->SpriteTemplate->LODLevels.Add(LOD);
    Emitter->CurrentLODLevelIndex = 0;
    Emitter->CurrentLODLevel = LOD;
    Emitter->bEnabled = true;

}
