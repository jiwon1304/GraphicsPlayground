#include "ParticleModuleColorOverLife.h"
#include "Classes/Particles/ParticleHelper.h"
#include "Classes/Particles/ParticleEmitterInstances.h"
#include "Classes/Components/ParticleSystemComponent.h"
#include "Classes/Particles/ParticleEmitter.h"
#include "CoreUObject/UObject/Casts.h"
#include "Classes/Particles/ParticleLODLevel.h"
#include "Classes/Particles/ParticleSystem.h"
#include "Classes/Distributions/DistributionFloatUniform.h"
#include "Classes/Distributions/DistributionVectorUniform.h"
#include "CoreUObject/UObject/ObjectFactory.h"
void UParticleModuleColorOverLife::InitializeDefaults()
{

}

void UParticleModuleColorOverLife::PostInitProperties()
{
    Super::PostInitProperties();
    InitializeDefaults();
    bEnabled = true;
    bUpdateModule = true;
    bSpawnModule = true;
}

void UParticleModuleColorOverLife::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    InitializeDefaults();
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UParticleModuleColorOverLife::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT
    float t = Particle.RelativeTime; // 0.0 ~ 1.0

    FLinearColor FinalColor;
    FinalColor.Lerp(StartColor, EndColor, t);

    Particle.Color = FinalColor;
    Particle.BaseColor = FinalColor;
}

void UParticleModuleColorOverLife::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    if ((Owner == NULL) || (Owner->ActiveParticles <= 0) ||
        (Owner->ParticleData == NULL) || (Owner->ParticleIndices == NULL))
    {
        return;
    }

    BEGIN_UPDATE_LOOP;
    {
        float t = Particle.RelativeTime;

        FLinearColor FinalColor;
        FinalColor.Lerp(StartColor, EndColor, t);

        Particle.Color = FinalColor;
    }
    END_UPDATE_LOOP;
}
