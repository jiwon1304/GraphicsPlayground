#include "ParticleModuleColorOverLife.h"
#include "Particles/ParticleHelper.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Components/ParticleSystemComponent.h"
#include "Particles/ParticleEmitter.h"
#include "UObject/Casts.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleSystem.h"
#include "Distributions/DistributionFloatUniform.h"
#include "Distributions/DistributionVectorUniform.h"
#include "UObject/ObjectFactory.h"
void UParticleModuleColorOverLife::InitializeDefaults()
{
    if (!ColorOverLife.IsCreated())
    {
        ColorOverLife.Distribution = FObjectFactory::ConstructObject<UDistributionVectorUniform>(this, TEXT("DistributionColorOverLife"));
    }

    if (!AlphaOverLife.IsCreated())
    {
        UDistributionFloatUniform* DistributionAlphaOverLife = FObjectFactory::ConstructObject<UDistributionFloatUniform>(this, TEXT("DistributionAlphaOverLife"));
        DistributionAlphaOverLife->Min = 1.0f;
        DistributionAlphaOverLife->Max = 1.0f;
        AlphaOverLife.Distribution = DistributionAlphaOverLife;
    }
}

void UParticleModuleColorOverLife::PostInitProperties()
{
    Super::PostInitProperties();
    InitializeDefaults();
    bEnabled = true;
}

void UParticleModuleColorOverLife::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    InitializeDefaults();
    //Curve Editor에서 필요
    //FProperty* PropertyThatChanged = PropertyChangedEvent.Property;
    //if (PropertyThatChanged)
    //{
    //    if (PropertyThatChanged->Name == (TEXT("bClampAlpha")))
    //    {
    //        UObject* OuterObj = GetOuter();
    //        assert(OuterObj!=nullptr);
    //        UParticleLODLevel* LODLevel = Cast<UParticleLODLevel>(OuterObj);
    //        if (LODLevel)
    //        {
    //            OuterObj = LODLevel->GetOuter();
    //            UParticleEmitter* Emitter = Cast<UParticleEmitter>(OuterObj);
    //            assert(Emitter!=nullptr);
    //            OuterObj = Emitter->GetOuter();
    //        }
    //        UParticleSystem* PartSys = CastChecked<UParticleSystem>(OuterObj);
    //        
    //        //PartSys->UpdateColorModuleClampAlpha(this);
    //    }
    //}
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UParticleModuleColorOverLife::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT
    FVector ColorVec = ColorOverLife.GetValue(Particle.RelativeTime, Owner->Component,0,nullptr);
    float	fAlpha = AlphaOverLife.GetValue(Particle.RelativeTime, Owner->Component);
    Particle.Color.R = (ColorVec.X);
    Particle.BaseColor.R = (ColorVec.X);
    Particle.Color.G = (ColorVec.Y);
    Particle.BaseColor.G = (ColorVec.Y);
    Particle.Color.B = (ColorVec.Z);
    Particle.BaseColor.B = (ColorVec.Z);
    Particle.Color.A = fAlpha;
    Particle.BaseColor.A = fAlpha;
}

void UParticleModuleColorOverLife::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    if ((Owner == NULL) || (Owner->ActiveParticles <= 0) ||
        (Owner->ParticleData == NULL) || (Owner->ParticleIndices == NULL))
    {
        return;
    }
    const FRawDistribution* FastColorOverLife = ColorOverLife.GetFastRawDistribution();
    const FRawDistribution* FastAlphaOverLife = AlphaOverLife.GetFastRawDistribution();
    if (FastColorOverLife && FastAlphaOverLife)
    {
        // fast path
        BEGIN_UPDATE_LOOP;
        {
            FastColorOverLife->GetValue3None(Particle.RelativeTime, &Particle.Color.R);
            FastAlphaOverLife->GetValue1None(Particle.RelativeTime, &Particle.Color.A);
        }
        END_UPDATE_LOOP;
    }
    else
    {
        FVector ColorVec;
        float	fAlpha;
        BEGIN_UPDATE_LOOP;
        {
            ColorVec = ColorOverLife.GetValue(Particle.RelativeTime, Owner->Component);
            fAlpha = AlphaOverLife.GetValue(Particle.RelativeTime, Owner->Component);
            Particle.Color.R = (ColorVec.X);
            Particle.Color.G = (ColorVec.Y);
            Particle.Color.B = (ColorVec.Z);
            Particle.Color.A = fAlpha;
        }
        END_UPDATE_LOOP;
    }
}
