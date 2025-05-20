#pragma once
#include "Particles/ParticleModules/ParticleModule.h"
#include "Math/Vector.h"
#include "Math/Quat.h"
#include "Math/RandomStream.h"

class UMaterial;
enum class EParticleUVFlipMode : uint8
{
    /** Flips UV on all particles. */
    None,
    /** Flips UV on all particles. */
    FlipUV,
    /** Flips U only on all particles. */
    FlipUOnly,
    /** Flips V only on all particles. */
    FlipVOnly,
    /** Flips UV randomly for each particle on spawn. */
    RandomFlipUV,
    /** Flips U only randomly for each particle on spawn. */
    RandomFlipUOnly,
    /** Flips V only randomly for each particle on spawn. */
    RandomFlipVOnly,
    /** Flips U and V independently at random for each particle on spawn. */
    RandomFlipUVIndependent,
};

/** Flips the sign of a particle's base size based on it's UV flip mode. */
FORCEINLINE void AdjustParticleBaseSizeForUVFlipping(FVector& OutSize, EParticleUVFlipMode FlipMode, const FRandomStream& InRandomStream)
{
    static constexpr float HalfRandMax = 0.5f;

    switch (FlipMode)
    {
    case EParticleUVFlipMode::None:
        return;

    case EParticleUVFlipMode::FlipUV:
        OutSize = -OutSize;
        return;

    case EParticleUVFlipMode::FlipUOnly:
        OutSize.X = -OutSize.X;
        return;

    case EParticleUVFlipMode::FlipVOnly:
        OutSize.Y = -OutSize.Y;
        return;

    case EParticleUVFlipMode::RandomFlipUV:
        OutSize = InRandomStream.FRand() > HalfRandMax ? -OutSize : OutSize;
        return;

    case EParticleUVFlipMode::RandomFlipUOnly:
        OutSize.X = InRandomStream.FRand() > HalfRandMax ? -OutSize.X : OutSize.X;
        return;

    case EParticleUVFlipMode::RandomFlipVOnly:
        OutSize.Y = InRandomStream.FRand() > HalfRandMax ? -OutSize.Y : OutSize.Y;
        return;

    case EParticleUVFlipMode::RandomFlipUVIndependent:
        OutSize.X = InRandomStream.FRand() > HalfRandMax ? -OutSize.X : OutSize.X;
        OutSize.Y = InRandomStream.FRand() > HalfRandMax ? -OutSize.Y : OutSize.Y;
        return;

    default:
        std::unreachable();
        break;
    }
}

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

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        EParticleUVFlipMode, UVFlippingMode
    )
    
    UPROPERTY(
        EditAnywhere,
        UMaterial*, Material, = nullptr;
    )
};
