#pragma once
#include "Particles/ParticleModules/ParticleModule.h"
#include "Math/Vector.h"
#include "Math/Quat.h"

class UMaterial;

class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)

public:
    UParticleModuleRequired() = default;
    virtual ~UParticleModuleRequired() override = default;

    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

public:
    UPROPERTY(
        EditAnywhere,
        float, EmitterDelay, = 0.0f; // Delay before the emitter starts
    )

    UPROPERTY(
        EditAnywhere,
        float, EmitterDuration, = 0.0f;
    )

    UPROPERTY(
        EditAnywhere,
        int32, EmitterLoops, = 0; // Number of times the emitter loops
    )

    uint8 bUseLocalSpace : 1;

    UPROPERTY(
        EditAnywhere,
        FVector, EmitterOrigin, = FVector::ZeroVector; // Origin of the emitter
    )

    UPROPERTY(
        EditAnywhere,
        FRotator, EmitterRotation, = FRotator::ZeroRotator; // Rotation of the emitter
    )

    UPROPERTY(
        EditAnywhere,
        UMaterial*, Material, = nullptr;
    )
};
