#pragma once
#include "ParticleEmitter.h"

// 새로운 에미터를 만들면 기본적으로 ParticleSpriteEmitter 형태로 만들어진다.
class UParticleSpriteEmitter : public UParticleEmitter
{
    DECLARE_CLASS(UParticleSpriteEmitter, UParticleEmitter)
public:
    UParticleSpriteEmitter() = default;
    virtual ~UParticleSpriteEmitter() override = default;

    virtual FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent) override;

    // !TODO : 기본값 세팅 함수
};
