#include "ParticleEmitter.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Components/ParticleSystemComponent.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModule.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleHelper.h"

FParticleEmitterInstance* UParticleEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    UE_LOG(ELogLevel::Error, TEXT("UParticleEmitter::CreateInstance is pure virtual"));
    return nullptr;
}

UParticleLODLevel* UParticleEmitter::GetCurrentLODLevel(FParticleEmitterInstance* Instance)
{
    // !NOTE : 지금은 LOD레벨 1개
    return Instance->CurrentLODLevel;
}

void UParticleEmitter::Build()
{
    CacheEmitterModuleInfo();
}

void UParticleEmitter::CacheEmitterModuleInfo()
{
    ParticleSize = sizeof(FBaseParticle);
}

