#pragma once
#include "ParticleModuleColorBase.h"
#include "Distributions/DistributionVector.h"
#include "Distributions/DistributionFloat.h"

class FPropertyChangedEvent;
class UParticleModuleColorOverLife :
    public UParticleModuleColorBase
{
    DECLARE_CLASS(UParticleModuleColorOverLife, UParticleModuleColorBase)
public:
    UParticleModuleColorOverLife() = default;
    virtual ~UParticleModuleColorOverLife() override = default;
    
    UPROPERTY_WITH_FLAGS(
        EditAnywhere | EditInline,
        FRawDistributionVector, ColorOverLife
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere | EditInline,
        FRawDistributionVector, AlphaOverLife
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        uint32, ColorOverLifeCount
    )

    void InitializeDefaults();
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;
};

