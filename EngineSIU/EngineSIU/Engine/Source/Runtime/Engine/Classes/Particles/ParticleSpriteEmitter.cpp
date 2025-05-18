#include "ParticleSpriteEmitter.h"
#include "ParticleLODLevel.h"
#include "ParticleModules/ParticleModuleTypeDataBase.h"
#include "ParticleEmitterInstances.h"

FParticleEmitterInstance* UParticleSpriteEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    FParticleEmitterInstance* Instance = 0;

    UParticleLODLevel* LODLevel = GetLODLevel(0);
    assert(LODLevel);

    if (LODLevel->TypeDataModule)
    {
        Instance = LODLevel->TypeDataModule->CreateInstance(this, InComponent);
    }
    else
    {
        assert(InComponent);
        Instance = new FParticleSpriteEmitterInstance();
        assert(Instance);

        Instance->InitParameters(this, InComponent);
    }

    if (Instance)
    {
        Instance->CurrentLODLevelIndex = 0;
        Instance->CurrentLODLevel = LODLevels[Instance->CurrentLODLevelIndex];
        Instance->Init();
    }

    return Instance;
}
