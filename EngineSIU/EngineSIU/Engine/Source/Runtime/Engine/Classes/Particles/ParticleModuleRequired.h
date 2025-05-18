#pragma once
#include "Particles/ParticleModule.h"
#include "Math/Vector.h"
#include "Math/Quat.h"

class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)
public:
    UParticleModuleRequired();
    virtual ~UParticleModuleRequired() override = default;

    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;
    float EmitterDelay = 0.0f; // Delay before the emitter starts
    float EmitterDuration = 0.0f;
    int32 EmitterLoops = 0; // Number of times the emitter loops

    uint8 bUseLocalSpace : 1;
    FVector EmitterOrigin = FVector::ZeroVector; // Origin of the emitter
    FRotator EmitterRotation = FRotator::ZeroRotator; // Rotation of the emitter
};
