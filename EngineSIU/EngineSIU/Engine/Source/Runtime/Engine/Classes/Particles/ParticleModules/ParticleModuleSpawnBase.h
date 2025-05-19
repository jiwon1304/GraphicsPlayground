#pragma once
#include "Particles/ParticleModules/ParticleModule.h"


class UParticleModuleSpawnBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSpawnBase, UParticleModule)

public:
    UParticleModuleSpawnBase() = default;
    virtual ~UParticleModuleSpawnBase() override = default;


    uint8 bProcessSpawnRate : 1;

    virtual bool GetSpawnAmount(
        FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,
        float DeltaTime, int32& Number, float& Rate
    )
    {
        return bProcessSpawnRate;
    }
};
