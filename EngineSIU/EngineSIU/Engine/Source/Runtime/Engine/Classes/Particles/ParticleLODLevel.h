#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleModuleRequired;
class UParticleModule;
class UParticleModuleTypeDataBase;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:
    UParticleLODLevel();
private:
    int32 Level;
    bool bEnabled;

    UParticleModuleRequired* RequiredModule;
    TArray<UParticleModule*> Modules;
    UParticleModuleTypeDataBase* TypeDataModule;
};

