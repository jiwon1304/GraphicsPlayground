#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FParticleEmitterInstance;
struct FBaseParticle;

class UParticleModule : public UObject
{
    DECLARE_CLASS(UParticleModule, UObject)
public:
    UParticleModule() = default;
    virtual ~UParticleModule() override = default;

    uint8 bEnabled : 1;
    uint8 bUpdateModule : 1;

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase);
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    virtual void FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
};
