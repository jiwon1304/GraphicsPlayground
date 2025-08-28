#include "ParticleModuleSize.h"

#include "ParticleModuleRequired.h"
#include "Components/ParticleSystemComponent.h"
#include "Distributions/DistributionVectorUniform.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleHelper.h"
#include "Particles/ParticleLODLevel.h"
#include "UObject/ObjectFactory.h"


void UParticleModuleSize::PostInitProperties()
{
    Super::PostInitProperties();
    InitializeDefaults();
    bEnabled = true;
    bSpawnModule = true;
}

void UParticleModuleSize::InitializeDefaults()
{
    if (!StartSize.IsCreated())
    {
        UDistributionVectorUniform* DistributionStartSize =
            FObjectFactory::ConstructObject<UDistributionVectorUniform>(this, TEXT("DistributionStartSize"));
        DistributionStartSize->Min = FVector(1.0f, 1.0f, 1.0f);
        DistributionStartSize->Max = FVector(1.0f, 1.0f, 1.0f);
        StartSize.Distribution = DistributionStartSize;
    }
}

void UParticleModuleSize::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SpawnEx(Owner, Offset, SpawnTime, &GetRandomStream(Owner), ParticleBase);
}

void UParticleModuleSize::SpawnEx(
    FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FRandomStream* InRandomStream, FBaseParticle* ParticleBase
)
{
    SPAWN_INIT

    FVector Size = StartSize.GetValue(Owner->EmitterTime, Owner->Component, 0, InRandomStream);
    Particle.Size += Size;

    AdjustParticleBaseSizeForUVFlipping(Size, Owner->CurrentLODLevel->RequiredModule->UVFlippingMode, *InRandomStream);
    Particle.BaseSize += Size;
}
