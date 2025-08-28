#pragma once
#include "ParticleModule.h"


class UParticleModuleVelocityBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleVelocityBase, UParticleModule)

public:
    UParticleModuleVelocityBase() = default;
    virtual ~UParticleModuleVelocityBase() override = default;

public:
    /**
     *	If true, then treat the velocity as world-space defined.
     *	NOTE: LocalSpace emitters that are moving will see strange results...
     */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        bool, bInWorldSpace
        // uint32 bInWorldSpace : 1;
    )

    /** If true, then apply the particle system components scale to the velocity value. */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        bool, bApplyOwnerScale
        // uint32 bApplyOwnerScale : 1;
    )
};
