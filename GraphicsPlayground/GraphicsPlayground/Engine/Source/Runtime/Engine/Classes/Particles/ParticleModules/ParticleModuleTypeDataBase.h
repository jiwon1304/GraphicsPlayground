#pragma once
#include "ParticleModule.h"

class UParticleEmitter;
class UParticleSystemComponent;


class UParticleModuleTypeDataBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleTypeDataBase, UParticleModule)

public:
    UParticleModuleTypeDataBase() = default;
    virtual ~UParticleModuleTypeDataBase() override = default;

    virtual FParticleEmitterInstance* CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent);
    virtual void PostInitProperties() override;
    virtual EModuleType GetModuleType() const override
    {
        return EPMT_TypeData;
    }

    virtual FName GetModuleName() const override
    {
        return FName(TEXT("TypeData"));
    }
};
