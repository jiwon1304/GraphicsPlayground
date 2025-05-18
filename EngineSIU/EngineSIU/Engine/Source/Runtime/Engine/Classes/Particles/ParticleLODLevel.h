#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleModule;
class UParticleModuleSpawnBase;
class UParticleModuleSpawn;
class UParticleModuleRequired;
class UParticleModuleTypeDataBase;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:
    UParticleLODLevel() = default;
    virtual ~UParticleLODLevel() override = default;
    void Initialize();

    int32 Level = 0;
    uint32 bEnabled : 1;

    virtual void UpdateModuleLists();

    UParticleModuleRequired* RequiredModule = nullptr;

    // 모든 모듈들
    TArray<UParticleModule*> Modules;

    // 스폰된 모듈들
    TArray<UParticleModule*> SpawnModules;

    // 업데이트 대상  모듈들
    TArray<UParticleModule*> UpdateModules;


    TArray<UParticleModuleSpawnBase*> SpawningModules;

    // SpawnRate/Burst 모듈. 모든 에미터에 공통으로 필요함
    class UParticleModuleSpawn* SpawnModule = nullptr;
    class UParticleModuleTypeDataBase* TypeDataModule = nullptr;
};
