#pragma once
#include "ParticleModuleSpawnBase.h"


class UParticleModuleSpawn : public UParticleModuleSpawnBase
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModuleSpawnBase)

public:
    UParticleModuleSpawn() = default;
    virtual ~UParticleModuleSpawn() override = default;

    // SpawnRate를 계산하는 함수
    virtual bool GetSpawnAmount(
        FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,
        float DeltaTime, int32& Number, float& Rate
    ) override
    {
        // !TODO : SpawnRate를 계산하는 로직을 구현합니다.
        Number = FMath::FloorToInt(SpawnRate * DeltaTime);
        Rate = SpawnRate;
        return true;
    }

    virtual EModuleType GetModuleType() const override
    {
        return EPMT_Spawn;
    }

    UPROPERTY(
        EditAnywhere,
        float, SpawnRate, = 20.f;
    )

    UPROPERTY(
        EditAnywhere,
        float, SpawnScale, = 1.0f;
    )

    float GetSpawnRate() const
    {
        // !TODO : FRawDistribution이 없기 때문에 외부에서 작성해서 그냥 넣어주기
        return SpawnRate;
    }

    float GetSpawnScale() const
    {
        // !TODO : FRawDistribution이 없기 때문에 외부에서 작성해서 그냥 넣어주기
        return SpawnScale;
    }
};
