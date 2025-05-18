#include "ParticleSystemComponent.h"

#include "ParticleEmitterInstance.h"
#include "ParticleHelper.h"
#include "Engine/Engine.h"

UFXSystemComponent::UFXSystemComponent()
{
}

UParticleSystemComponent::UParticleSystemComponent()
{
}

void UParticleSystemComponent::InitTestSpriteParticles()
{
    // [1] 더미 EmitterInstance 생성
    FParticleEmitterInstance* Emitter = new FParticleEmitterInstance();

    Emitter->SpriteTemplate = nullptr;
    Emitter->Component = this;

    Emitter->ParticleStride = sizeof(FBaseParticle);
    Emitter->MaxActiveParticles = 64;
    Emitter->ActiveParticles = 0;

    // [2] 메모리 할당
    const int32 ParticleBytes = Emitter->ParticleStride * Emitter->MaxActiveParticles;
    const int32 IndexCount = Emitter->MaxActiveParticles;

    Emitter->ParticleData = new uint8[ParticleBytes];
    std::memset(Emitter->ParticleData, 0, ParticleBytes);

    Emitter->ParticleIndices = new uint16[IndexCount];
    std::memset(Emitter->ParticleIndices, 0, IndexCount * sizeof(uint16));

    // [3] 등록
    EmitterInstances.Add(Emitter);

    // [4] 테스트 파티클 생성
    const int32 ParticleCount = 20;
    const FVector Positions[ParticleCount] = {
        { 0,  0,  0}, {10,  5,  2}, {20, -4,  3}, {30,  2,  1}, {40, -6,  5},
        { 5, 12,  0}, {15,  9,  3}, {25, -3,  2}, {35,  4,  4}, {45, -5,  6},
        { 0, -8,  1}, {10, -9,  2}, {20,  6,  0}, {30,  7,  3}, {40,  0,  1},
        { 5,  3,  4}, {15, -2,  2}, {25,  8,  5}, {35, -7,  3}, {45,  1,  6},
    };

    const FLinearColor Colors[] = {
        FLinearColor::Red, FLinearColor::Green, FLinearColor::Blue,
        FLinearColor::Yellow
    };

    for (int32 i = 0; i < ParticleCount; ++i)
    {
        uint8* Ptr = Emitter->ParticleData + i * Emitter->ParticleStride;
        FBaseParticle* Particle = reinterpret_cast<FBaseParticle*>(Ptr);

        Particle->Location = Positions[i];
        Particle->OldLocation = Positions[i];
        Particle->Size = FVector(5.0f);
        Particle->BaseSize = Particle->Size;
        Particle->Color = Colors[i % 4];
        Particle->BaseColor = Particle->Color;
        Particle->RelativeTime = 0.5f;
        Particle->OneOverMaxLifetime = 1.0f;
        Particle->Rotation = 0.0f;
        Particle->RotationRate = 0.0f;
        Particle->BaseRotationRate = 0.0f;
        Particle->Velocity = FVector::ZeroVector;
        Particle->BaseVelocity = FVector::ZeroVector;
        Particle->Flags = 0;
        Particle->Placeholder0 = Particle->Placeholder1 = 0.0f;

        Emitter->ParticleIndices[i] = i;
    }

    Emitter->ActiveParticles = ParticleCount;

    // [5] 더미 EmitterRenderData도 생성
    FDynamicSpriteEmitterReplayData* ReplayData = new FDynamicSpriteEmitterReplayData();
    ReplayData->eEmitterType = EDynamicEmitterType::DET_Sprite;
    ReplayData->ActiveParticleCount = ParticleCount;
    ReplayData->ParticleStride = Emitter->ParticleStride;
    ReplayData->DataContainer.ParticleData = Emitter->ParticleData;
    ReplayData->DataContainer.ParticleIndices = Emitter->ParticleIndices;

    FDynamicSpriteEmitterData* SpriteEmitterData = new FDynamicSpriteEmitterData(nullptr);
    SpriteEmitterData->Source = *ReplayData;
    SpriteEmitterData->bValid = true;

    EmitterRenderData.Add(SpriteEmitterData);
}
