#pragma once
#include "Components/Material/Material.h"
#include "Math/Vector.h"
#include "Math/Color.h"

class UParticleModuleRequired;
struct FBaseParticle
{
    // 24 bytes
    FVector OldLocation;      // Last frame's location, used for collision
    FVector Location;         // Current location

    // 16 bytes
    FVector BaseVelocity;     // Velocity = BaseVelocity at the start of each frame.
    float Rotation;           // Rotation of particle (in Radians)

    // 16 bytes
    FVector Velocity;         // Current velocity, gets reset to BaseVelocity each frame to allow 
    float BaseRotationRate;   // Initial angular velocity of particle (in Radians per second)

    // 16 bytes
    FVector BaseSize;         // Size = BaseSize at the start of each frame
    float RotationRate;       // Current rotation rate, gets reset to BaseRotationRate each frame

    // 16 bytes
    FVector Size;             // Current size, gets reset to BaseSize each frame
    int32 Flags;              // Flags indicating various particle states

    // 16 bytes
    FLinearColor Color;       // Current color of particle.

    // 16 bytes
    FLinearColor BaseColor;   // Base color of the particle

    // 16 bytes
    float RelativeTime;       // Relative time, range is 0 (==spawn) to 1 (==death)
    float OneOverMaxLifetime; // Reciprocal of lifetime
    float Placeholder0;
    float Placeholder1;
};

enum EParticleStates
{
    /** Ignore updates to the particle */
    STATE_Particle_JustSpawned = 0x02000000,
    /** Ignore updates to the particle */
    STATE_Particle_Freeze = 0x04000000,
    /** Ignore collision updates to the particle */
    STATE_Particle_IgnoreCollisions = 0x08000000,
    /** Stop translations of the particle */
    STATE_Particle_FreezeTranslation = 0x10000000,
    /** Stop rotations of the particle */
    STATE_Particle_FreezeRotation = 0x20000000,
    /** Combination for a single check of 'ignore' flags */
    STATE_Particle_CollisionIgnoreCheck = STATE_Particle_Freeze | STATE_Particle_IgnoreCollisions | STATE_Particle_FreezeTranslation | STATE_Particle_FreezeRotation,
    /** Delay collision updates to the particle */
    STATE_Particle_DelayCollisions = 0x40000000,
    /** Flag indicating the particle has had at least one collision */
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

    void Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts);
    void Free();
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
    virtual ~FDynamicEmitterDataBase() = default;

public:
    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;

    int32  EmitterIndex;
};

struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{
    FDynamicSpriteEmitterDataBase(const UParticleModuleRequired* RequiredModule) :
        FDynamicEmitterDataBase(RequiredModule)
    {
    }

public:
    void SortSpriteParticles()
    {
    }
};

class UMaterial;
struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
    UMaterial* Material = nullptr;

    FDynamicSpriteEmitterReplayDataBase()
    {
        eEmitterType = DET_Sprite;
        //테스트용 하드코딩 경로
        FTextureInfo TexInfo;
        TexInfo.TextureName = TEXT("T_Explosion_SubUV");
        TexInfo.TexturePath = L"Assets/Texture/T_Explosion_SubUV.png"; // 경로는 Wide
        TexInfo.bIsSRGB = true;
        if (!Material)Material = new UMaterial();
        Material->GetMaterialInfo().TextureInfos.Add(TexInfo);
    }
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

public:
    void Init() {}

    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    FDynamicSpriteEmitterReplayData Source;
};

struct FDynamicMeshEmitterReplayDataBase : FDynamicEmitterReplayDataBase
{
    FDynamicMeshEmitterReplayDataBase()
    {
        eEmitterType = DET_Mesh;
    }
};

struct FDynamicMeshEmitterReplayData : public FDynamicMeshEmitterReplayDataBase
{
};

struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterDataBase
{
    // TODO: 생성자 만들기
    FDynamicMeshEmitterData();

    void Init() {}
    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    FDynamicMeshEmitterReplayData Source;
};

template <typename T>
FORCEINLINE constexpr T Align(T Val, uint64 Alignment)
{
    static_assert(std::is_integral_v<T> || std::is_pointer_v<T>, "Align expects an integer or pointer type");

    return static_cast<T>((static_cast<uint64>(Val) + Alignment - 1) & ~(Alignment - 1));
}

/*-----------------------------------------------------------------------------
    Helper macros.
-----------------------------------------------------------------------------*/
#define DECLARE_PARTICLE(Name,Address) \
    FBaseParticle& Name = *((FBaseParticle*) (Address));

#define DECLARE_PARTICLE_CONST(Name,Address) \
    const FBaseParticle& Name = *((const FBaseParticle*) (Address));

#define DECLARE_PARTICLE_PTR(Name,Address) \
    FBaseParticle* Name = (FBaseParticle*) (Address);

#define BEGIN_UPDATE_LOOP \
    { \
        assert((Owner != NULL) && (Owner->Component != NULL)); \
        int32& ActiveParticles = Owner->ActiveParticles; \
        uint32 CurrentOffset = Offset; \
        const uint8* ParticleData = Owner->ParticleData; \
        const uint32 ParticleStride = Owner->ParticleStride; \
        uint16* ParticleIndices = Owner->ParticleIndices; \
        for(int32 i=ActiveParticles-1; i>=0; i--) \
        { \
            const int32 CurrentIndex = ParticleIndices[i]; \
            const uint8* ParticleBase = ParticleData + CurrentIndex * ParticleStride; \
            FBaseParticle& Particle = *((FBaseParticle*) ParticleBase); \
            if ((Particle.Flags & STATE_Particle_Freeze) == 0) \
            {
#define END_UPDATE_LOOP \
            } \
            CurrentOffset = Offset; \
        } \
    }

#define CONTINUE_UPDATE_LOOP \
        CurrentOffset = Offset; \
        continue;

#define SPAWN_INIT \
    assert((Owner != NULL) && (Owner->Component != NULL)); \
    const int32 ActiveParticles = Owner->ActiveParticles; \
    const uint32 ParticleStride = Owner->ParticleStride; \
    uint32 CurrentOffset = Offset; \
    FBaseParticle& Particle = *(ParticleBase);

#define PARTICLE_ELEMENT(Type,Name) \
    Type& Name = *((Type*)((uint8*)ParticleBase + CurrentOffset)); \
    CurrentOffset += sizeof(Type);

#define KILL_CURRENT_PARTICLE \
    { \
        ParticleIndices[i] = ParticleIndices[ActiveParticles-1]; \
        ParticleIndices[ActiveParticles-1] = CurrentIndex; \
        ActiveParticles--; \
    }
