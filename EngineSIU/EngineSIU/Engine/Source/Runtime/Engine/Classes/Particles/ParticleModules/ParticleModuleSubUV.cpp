#include "ParticleModuleSubUV.h"
#include "Particles/ParticleEmitterInstances.h"

void UParticleModuleSubUV::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    const int PayloadOffset = Owner->SubUVDataOffset;
}

void UParticleModuleSubUV::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    
}
