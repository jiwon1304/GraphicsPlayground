#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleLODLevel;
class UParticleSystemComponent;
class UParticleModule;
struct FParticleEmitterInstance;

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter() = default;
    virtual ~UParticleEmitter() = default;
    
    virtual FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent);
    UParticleLODLevel* GetCurrentLODLevel(FParticleEmitterInstance* Instance);

    void Build();
    void CacheEmitterModuleInfo();

    FName EmitterName;
    int32 ParticleSize = 0;

    TArray<UParticleLODLevel*> LODLevels;

    TMap<UParticleModule*, uint32> ModuleOffsetMap;
};
