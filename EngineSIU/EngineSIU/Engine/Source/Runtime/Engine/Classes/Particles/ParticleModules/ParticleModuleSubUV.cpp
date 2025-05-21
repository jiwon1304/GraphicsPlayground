#include "ParticleModuleSubUV.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleEmitter.h"

void UParticleModuleSubUV::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    UParticleLODLevel* LODLevel = Owner->SpriteTemplate->GetCurrentLODLevel(Owner);
    const int32 PayloadOffset = Owner->SubUVDataOffset;

    SPAWN_INIT;
    {
        int32 TempOffset = CurrentOffset;
        CurrentOffset = PayloadOffset;
        PARTICLE_ELEMENT(FSubUVPayload, SubUVPayload);
        CurrentOffset = TempOffset;

        SubUVPayload.SubImages_Horizontal = SubImages_Horizontal;
        SubUVPayload.SubImages_Vertical = SubImages_Vertical;
    }
}

void UParticleModuleSubUV::PostInitProperties()
{
    bEnabled = true;
    bSpawnModule = true;
}

void UParticleModuleSubUV::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    
}
