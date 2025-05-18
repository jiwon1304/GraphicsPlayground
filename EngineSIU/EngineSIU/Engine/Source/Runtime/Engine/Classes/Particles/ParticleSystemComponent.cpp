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
    static const FVector2D UVs[6] = {
        {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f},
        {-0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}
    };

    SpriteVertexData.SetNum(6);

    for (int i = 0; i < 6; ++i)
    {
        FParticleSpriteVertex& V = SpriteVertexData[i];
        V.Position = FVector::ZeroVector;
        V.RelativeTime = 0.5f;
        V.OldPosition = FVector::ZeroVector;
        V.ParticleId = 0.0f;
        V.Size = FVector2D(100.0f, 100.0f);
        V.Rotation = 0.0f;
        V.SubImageIndex = 0.0f;
        V.Color = FLinearColor::Red;
        V.UV = UVs[i];
    }
}
