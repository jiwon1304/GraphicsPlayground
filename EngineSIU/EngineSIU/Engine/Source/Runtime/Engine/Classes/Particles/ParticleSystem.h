#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleSystemComponent;
class UParticleEmitter;


// !TODO : fxasset
class UParticleSystem : public UObject
{
    DECLARE_CLASS(UParticleSystem, UObject)

public:
    UParticleSystem() = default;
    virtual ~UParticleSystem() override = default;

    // 에디터에서 변동사항이 생겼다면 이 함수 호출
    virtual void PostEditChangeProperty();

    TArray<UParticleEmitter*> Emitters;

    // 에디터용이지만 상호참조하도록 일단 할당해주기
    UPROPERTY(
        Transient,
        UParticleSystemComponent*, PreviewComponent, = nullptr;
    )

    void BuildEmitters();
};
