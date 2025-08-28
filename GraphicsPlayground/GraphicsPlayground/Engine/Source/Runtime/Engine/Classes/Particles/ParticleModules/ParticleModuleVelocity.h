#pragma once
#include "ParticleModuleVelocityBase.h"
#include "Distributions/DistributionFloat.h"
#include "Distributions/DistributionVector.h"


class UParticleModuleVelocity : public UParticleModuleVelocityBase
{
    DECLARE_CLASS(UParticleModuleVelocity, UParticleModuleVelocityBase)

public:
    UParticleModuleVelocity();
    virtual ~UParticleModuleVelocity() override = default;

    /** 
     *	The velocity to apply to a particle when it is spawned.
     *	Value is retrieved using the EmitterTime of the emitter.
     */
    UPROPERTY(
        EditAnywhere,
        FRawDistributionVector, StartVelocity, {};
    )

    /** 
     *	The velocity to apply to a particle along its radial direction.
     *	Direction is determined by subtracting the location of the emitter from the particle location at spawn.
     *	Value is retrieved using the EmitterTime of the emitter.
     */
    UPROPERTY(
        EditAnywhere,
        FRawDistributionFloat, StartVelocityRadial, {}
    )

public:
    /** Initializes the default values for this property */
    void InitializeDefaults();

    //~ Begin UObject Interface
#if WITH_EDITOR
    // virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
    virtual void PostInitProperties() override;
    //~ End UObject Interface

    //~ Begin UParticleModule Interface
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    //~ End UParticleModule Interface

    virtual FName GetModuleName() const override;

    /**
     *	Extended version of spawn, allows for using a random stream for distribution value retrieval
     *
     *	@param	Owner				The particle emitter instance that is spawning
     *	@param	Offset				The offset to the modules payload data
     *	@param	SpawnTime			The time of the spawn
     *	@param	InRandomStream		The random stream to use for retrieving random values
     */
    void SpawnEx(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, struct FRandomStream* InRandomStream, FBaseParticle* ParticleBase);
};
