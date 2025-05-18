#pragma once
#include "ParticleEmitter.h"

class UParticleSpriteEmitter : public UParticleEmitter
{
    DECLARE_CLASS(UParticleSpriteEmitter, UParticleEmitter)
public:
    UParticleSpriteEmitter() = default;
    virtual ~UParticleSpriteEmitter() override = default;

    virtual FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent) override;

    // !TODO : 기본값 세팅 함수
};
