#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Components/PrimitiveComponent.h"

// !TODO : 지금은 하는게 없음. 기능 추가되면 별개의 스크립트로 분리
class UFXSystemComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UFXSystemComponent, UPrimitiveComponent)
public:
    UFXSystemComponent() = default;
    virtual ~UFXSystemComponent() = default;
};

enum EParticleReplayState : int 
{
    PRS_Disabled = 0,
    PRS_Capturing = 1,
    PRS_Replaying = 2,
    PRS_MAX
};

class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)
public:
    UParticleSystemComponent() = default;
    virtual ~UParticleSystemComponent() = default;

    void InitializeSystem();
    void InitParticles();
    virtual void TickComponent(float DeltaTime) override;

    void SetTemplate(class UParticleSystem* InTemplate);

    class UParticleSystem* Template = nullptr;
    uint8 bWasCompleted : 1;
    uint8 bSuppressingSpawing : 1;

    int LODLevel;
    float EmitterDelay;

    TArray<struct FParticleEmitterInstance*> EmitterInstances;

};
