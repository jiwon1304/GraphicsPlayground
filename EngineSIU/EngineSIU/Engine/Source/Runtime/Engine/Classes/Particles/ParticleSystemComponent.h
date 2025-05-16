#pragma once
#include "Components/PrimitiveComponent.h"

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
private:
    TArray<struct FParticleEmitterInstance*> EmitterInstances;

    UParticleSystem* Template;
    TArray<class UMaterialInterface*> Materials;
    TArray<USkeletalMeshComponent*> SkelMeshComponents;

    TArray<FDynamicEmitterDataBase*> EmitterRenderData;

};

