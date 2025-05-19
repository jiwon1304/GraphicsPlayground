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

    UPROPERTY
    (int32, Level, = 0;)

    UPROPERTY_WITH_BITFIELD(
        BitField,
        uint32, bEnabled, : 1;
    )

    virtual void UpdateModuleLists();
    int32 GetModuleIndex(UParticleModule* InModule);

    UPROPERTY
    (UParticleModuleRequired*, RequiredModule, = nullptr;)

    // 모든 모듈들
    UPROPERTY
    (TArray<UParticleModule*>, Modules)

    // 스폰된 모듈들
    UPROPERTY_WITH_FLAGS(
        Transient | DuplicateTransient,
        TArray<UParticleModule*>, SpawnModules
    )

    // 업데이트 대상  모듈들
    UPROPERTY_WITH_FLAGS(
        Transient | DuplicateTransient,
        TArray<UParticleModule*>, UpdateModules
    )

    UPROPERTY_WITH_FLAGS(
        Transient | DuplicateTransient,
        TArray<UParticleModuleSpawnBase*>, SpawningModules
    )

    // SpawnRate/Burst 모듈. 모든 에미터에 공통으로 필요함
    UPROPERTY
    (UParticleModuleSpawn*, SpawnModule, = nullptr;)

    UPROPERTY
    (UParticleModuleTypeDataBase*, TypeDataModule, = nullptr;)
};
