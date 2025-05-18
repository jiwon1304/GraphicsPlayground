#pragma once
#include "ParticleHelper.h"
#include "Components/PrimitiveComponent.h"

struct FParticleSpriteVertex;
class UParticleSystem;
class USkeletalMeshComponent;
class FDynamicEmitterDataBase;

class UFXSystemComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UFXSystemComponent, UPrimitiveComponent)
public:
    UFXSystemComponent();
};


class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)
public:
    UParticleSystemComponent();

    inline TArray<FDynamicEmitterDataBase*>& GetEmitterRenderData()
    {
        return EmitterRenderData;
    }

    // [1] 현재 파티클이 어떤 타입인지 반환
    EDynamicEmitterType GetEmitterType() const
    {
        // 현재는 Sprite 기반으로 가정
        return EDynamicEmitterType::DET_Sprite;
    }

    void InitTestSpriteParticles();



private:
    TArray<struct FParticleEmitterInstance*> EmitterInstances;

    UParticleSystem* Template;
    TArray<class UMaterialInterface*> Materials;
    TArray<FDynamicEmitterDataBase*> EmitterRenderData;
};
