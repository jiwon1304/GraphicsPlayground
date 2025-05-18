#pragma once
#include "Container/Array.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

class UParticleEmitter;
class UParticleSystemComponent;
class UParticleLODLevel;
class UParticleModule;

struct FBaseParticle;
struct FVector;

struct FParticleEmitterInstance
{
    // 에디터에서 세팅한 ParticleEmitter 그 자체
    UParticleEmitter* SpriteTemplate = nullptr;
    UParticleSystemComponent* Component = nullptr;
    UParticleLODLevel* CurrentLODLevel = nullptr;
    int32 CurrentLODLevelIndex = 0;

    bool bEnabled = false;
    float EmitterDuration = 0.0f;
    TArray<float> EmitterDurations;
    float CurrentDelay = 0.0f;
    float EmitterTime = 0.0f;
    float LastDeltaTime = 0.0f;
    // EmitterInstance가 생성된 후 경과한 시간
    float SecondsSinceCreation = 0.0f;
    int32 ActiveParticles = 0;
    int32 LoopCount = 0;
    uint32 ParticleCounter = 0;

    // 한 번에 여러 개의 파티클이 생성될 때 정규화된 진행률
    float SpawnFraction = 0.0f;
    // 파티클 데이터의 시작 주소
    uint8* ParticleData = nullptr;
    int32 ParticleSize = 0;
    int32 ParticleStride = 0;
    int32 PayloadOffset = 0;

    uint16* ParticleIndices = nullptr;
    int32 MaxActiveParticles = 0;

    FVector PositionOffsetThisTick;

    FVector Location = FVector::ZeroVector;
    FVector OldLocation = FVector::ZeroVector;

    FMatrix EmitterToSimulation = FMatrix::Identity;
    FMatrix SimulationToWorld = FMatrix::Identity;
    virtual void InitParameters(UParticleEmitter* InTemplate, UParticleSystemComponent* InComponent);
    virtual void Init();

    // Tick관련
    virtual void Tick(float DeltaTime);
    virtual float Tick_EmitterTimeSetup(float DeltaTime, UParticleLODLevel* CurrentLODLevel);
    virtual float Tick_SpawnParticles(float DeltaTime, UParticleLODLevel* CurrentLODlevel, bool bFirstTime);
    virtual void Tick_ModuleUpdate(float DeltaTime, UParticleLODLevel* CurrentLODLevel);
    virtual void Tick_ModulePostUpdate(float DeltaTime, UParticleLODLevel* CurrentLODLevel);
    virtual void Tick_ModuleFinalUpdate(float DeltaTime, UParticleLODLevel* CurrentLODLevel);

    UParticleLODLevel* GetCurrentLODLevelChecked();
    uint32 GetModuleDataOffset(UParticleModule* Module);

    void SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity/*, struct FParticleEventInstancePayload* EventPayload */);

    virtual void PreSpawn(FBaseParticle* Particle, const FVector& InitialLocation, const FVector& InitialVelocity);
    virtual float Spawn(float DeltaTime);
    virtual void PostSpawn(FBaseParticle* Particle, float InterpolationPercentage, float SpawnTime);

    void UpdateTransforms();
    // 월드좌표 변경 시 호출
    virtual void ApplyWorldOffset(FVector InOffset, bool bWorldShift);
    bool Resize(int32 NewMaxActiveParticles, bool bSetMaxActiveCount);

    void KillParticles();
    void KillParticle(int32 Index);

    void SetupEmitterDuration();
};
