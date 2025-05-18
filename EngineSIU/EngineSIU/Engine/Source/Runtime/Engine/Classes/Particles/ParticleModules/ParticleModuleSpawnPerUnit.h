#pragma once
#include "ParticleModuleSpawnBase.h"

class UParticleModuleSpawnPerUnit : public UParticleModuleSpawnBase 
{
    DECLARE_CLASS(UParticleModuleSpawnPerUnit, UParticleModuleSpawnBase)
public:
    UParticleModuleSpawnPerUnit() = default;
    virtual ~UParticleModuleSpawnPerUnit() override = default;
    // SpawnRate를 계산하는 함수
    virtual bool GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,
        float DeltaTime, int32& Number, float& Rate) override;

};
