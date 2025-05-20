#include "ParticleModuleVelocity.h"

#include "ParticleModuleRequired.h"
#include "Components/ParticleSystemComponent.h"
#include "Distributions/DistributionFloatUniform.h"
#include "Distributions/DistributionVectorUniform.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleHelper.h"
#include "Particles/ParticleLODLevel.h"
#include "UObject/ObjectFactory.h"

UParticleModuleVelocity::UParticleModuleVelocity()
{
    bSpawnModule = true;
}

void UParticleModuleVelocity::InitializeDefaults()
{
    if (!StartVelocity.IsCreated())
    {
        StartVelocity.Distribution = FObjectFactory::ConstructObject<UDistributionVectorUniform>(this, TEXT("DistributionStartVelocity"));
    }

    if (!StartVelocityRadial.IsCreated())
    {
        StartVelocityRadial.Distribution = FObjectFactory::ConstructObject<UDistributionFloatUniform>(this, TEXT("DistributionStartVelocityRadial"));
    }
}

void UParticleModuleVelocity::PostInitProperties()
{
    Super::PostInitProperties();
    InitializeDefaults();
}

void UParticleModuleVelocity::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    // SpawnEx(Owner, Offset, SpawnTime, &GetRandomStream(Owner), ParticleBase);
    SpawnEx(Owner, Offset, SpawnTime, nullptr, ParticleBase);
}

void UParticleModuleVelocity::SpawnEx(
    FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FRandomStream* InRandomStream, FBaseParticle* ParticleBase
)
{
    SPAWN_INIT
    {
        FVector Vel = StartVelocity.GetValue(Owner->EmitterTime, Owner->Component, 0, InRandomStream);
        FVector FromOrigin = (Particle.Location - Owner->EmitterToSimulation.GetOrigin()).GetSafeNormal();

        FVector OwnerScale(1.0f);
        if ((bApplyOwnerScale == true) && Owner->Component)
        {
            OwnerScale = Owner->Component->GetComponentTransform().GetScale3D();
        }

        UParticleLODLevel* LODLevel = Owner->SpriteTemplate->GetCurrentLODLevel(Owner);
        assert(LODLevel);
        if (LODLevel->RequiredModule->bUseLocalSpace)
        {
            if (bInWorldSpace == true)
            {
                Vel = Owner->SimulationToWorld.InverseTransformVector(Vel);
            }
            else
            {
                Vel = Owner->EmitterToSimulation.TransformVector(Vel);
            }
        }
        else if (bInWorldSpace == false)
        {
            Vel = Owner->EmitterToSimulation.TransformVector(Vel);
        }
        Vel *= OwnerScale;
        Vel += FromOrigin * StartVelocityRadial.GetValue(Owner->EmitterTime, Owner->Component, InRandomStream) * OwnerScale;
        Particle.Velocity += Vel;
        Particle.BaseVelocity += Vel;
    }
}
