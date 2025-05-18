#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

// !TODO : fxasset
class UParticleSystem : public UObject
{
    DECLARE_CLASS(UParticleSystem, UObject)
public:
    UParticleSystem() = default;
    virtual ~UParticleSystem() override = default;

    TArray<class UParticleEmitter*> Emitters;
    void BuildEmitters();
};
