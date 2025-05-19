#pragma once
#include "Math/Vector.h"
#include "Math/Color.h"

class UParticleModuleRequired;
struct FBaseParticle
{
    FVector		OldLocation;
    FVector		Location;
    FVector		BaseVelocity;
    float		Rotation;
    FVector		Velocity;
    float		BaseRotationRate;
    FVector		BaseSize;
    float		RotationRate;
    FVector		Size;
    int32		Flags;
    FLinearColor	Color;
    FLinearColor	BaseColor;
    float		RelativeTime;
    float		OneOverMaxLifetime;
    float		Placeholder0;
    float		Placeholder1;
};

enum EParticleStates
{
    STATE_Particle_JustSpawned = 0x02000000,
    STATE_Particle_Freeze = 0x04000000,
    STATE_Particle_IgnoreCollisions = 0x08000000,
    STATE_Particle_FreezeTranslation = 0x10000000,
    STATE_Particle_FreezeRotation = 0x20000000,
    STATE_Particle_CollisionIgnoreCheck = STATE_Particle_Freeze | STATE_Particle_IgnoreCollisions | STATE_Particle_FreezeTranslation | STATE_Particle_FreezeRotation,
    STATE_Particle_DelayCollisions = 0x40000000,
    STATE_Particle_CollisionHasOccurred = 0x80000000,
    STATE_Mask = 0xFE000000,
    STATE_CounterMask = (~STATE_Mask)
};

enum EDynamicEmitterType
{
    DET_Unknown = 0,
    DET_Sprite,
    DET_Mesh,
    DET_Beam2,
    DET_Ribbon,
    DET_AnimTrail,
    DET_Custom
};

struct FParticleDataContainer
{
    int32 MemBlockSize;
    int32 ParticleDataNumBytes;
    int32 ParticleIndicesNumShorts;
    uint8* ParticleData;
    uint16* ParticleIndices;

    FParticleDataContainer()
        : MemBlockSize(0)
        , ParticleDataNumBytes(0)
        , ParticleIndicesNumShorts(0)
        , ParticleData(nullptr)
        , ParticleIndices(nullptr)
    {
    }
    ~FParticleDataContainer()
    {
        Free();
    }
    void Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts) {};
    void Free() {};
};

struct FDynamicEmitterReplayDataBase
{
    EDynamicEmitterType eEmitterType;
    int32 ActiveParticleCount;
    int32 ParticleStride;
    FParticleDataContainer DataContainer;
    FVector Scale;
    uint32 SortMode;

    FDynamicEmitterReplayDataBase()
        : eEmitterType(DET_Unknown),
        ActiveParticleCount(0),
        ParticleStride(0),
        Scale(FVector(1.0f, 1.0f, 1.0f)),
        SortMode(0)
    {
    }
};

struct FDynamicEmitterDataBase
{
    FDynamicEmitterDataBase(const UParticleModuleRequired* RequiredModule);

    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;
    int32  EmitterIndex;
};

struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{
    FDynamicSpriteEmitterDataBase(const UParticleModuleRequired* RequiredModule) :
        FDynamicEmitterDataBase(RequiredModule)
    {
    }

    virtual ~FDynamicSpriteEmitterDataBase()
    {
    }
    void SortSpriteParticles() {};
};

struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
};

struct FDynamicSpriteEmitterReplayData : public FDynamicSpriteEmitterReplayDataBase
{
};

struct FDynamicSpriteEmitterData : public FDynamicSpriteEmitterDataBase
{
    FDynamicSpriteEmitterData(const UParticleModuleRequired* RequiredModule) :
        FDynamicSpriteEmitterDataBase(RequiredModule)
    {
    }

    ~FDynamicSpriteEmitterData()
    {
    }
    void Init() {}
    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }
    FDynamicSpriteEmitterReplayData Source;
};

struct FDynamicMeshEmitterReplayDataBase : FDynamicEmitterReplayDataBase
{
};

struct FDynamicMeshEmitterReplayData : public FDynamicMeshEmitterReplayDataBase
{
};

struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterDataBase
{
    void Init() {}
    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }
    FDynamicMeshEmitterReplayData Source;
};

#define DECLARE_PARTICLE_PTR(Name,Address)		\
	FBaseParticle* Name = (FBaseParticle*) (Address);
