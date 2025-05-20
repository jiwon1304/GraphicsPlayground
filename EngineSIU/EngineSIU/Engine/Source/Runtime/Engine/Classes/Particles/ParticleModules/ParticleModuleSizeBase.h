#pragma once
#include "ParticleModule.h"


class UParticleModuleSizeBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSizeBase, UParticleModule)

public:
    UParticleModuleSizeBase() = default;
    virtual ~UParticleModuleSizeBase() override = default;

    virtual FName GetModuleName() const override
    {
        return FName(TEXT("SizeBase"));
    }
};
