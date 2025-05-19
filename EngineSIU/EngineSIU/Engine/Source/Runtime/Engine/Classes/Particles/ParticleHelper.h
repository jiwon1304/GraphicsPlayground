#pragma once
#include "Math/Vector.h"
#include "Math/Color.h"

struct FBaseParticle
{
    // 24 bytes
    FVector		OldLocation;			// Last frame's location, used for collision
    FVector		Location;				// Current location

    // 16 bytes
    FVector		    BaseVelocity;			// Velocity = BaseVelocity at the start of each frame.
    float			Rotation;				// Rotation of particle (in Radians)

    // 16 bytes
    FVector		    Velocity;				// Current velocity, gets reset to BaseVelocity each frame to allow 
    float			BaseRotationRate;		// Initial angular velocity of particle (in Radians per second)

    // 16 bytes
    FVector		    BaseSize;				// Size = BaseSize at the start of each frame
    float			RotationRate;			// Current rotation rate, gets reset to BaseRotationRate each frame

    // 16 bytes
    FVector		    Size;					// Current size, gets reset to BaseSize each frame
    int32			Flags;					// Flags indicating various particle states

    // 16 bytes
    FLinearColor	Color;					// Current color of particle.

    // 16 bytes
    FLinearColor	BaseColor;				// Base color of the particle

    // 16 bytes
    float			RelativeTime;			// Relative time, range is 0 (==spawn) to 1 (==death)
    float			OneOverMaxLifetime;		// Reciprocal of lifetime
    float			Placeholder0;
    float			Placeholder1;
};

/*-----------------------------------------------------------------------------
    Particle State Flags
-----------------------------------------------------------------------------*/
enum EParticleStates
{
    /** Ignore updates to the particle						*/
    STATE_Particle_JustSpawned = 0x02000000,
    /** Ignore updates to the particle						*/
    STATE_Particle_Freeze = 0x04000000,
    /** Ignore collision updates to the particle			*/
    STATE_Particle_IgnoreCollisions = 0x08000000,
    /**	Stop translations of the particle					*/
    STATE_Particle_FreezeTranslation = 0x10000000,
    /**	Stop rotations of the particle						*/
    STATE_Particle_FreezeRotation = 0x20000000,
    /** Combination for a single check of 'ignore' flags	*/
    STATE_Particle_CollisionIgnoreCheck = STATE_Particle_Freeze | STATE_Particle_IgnoreCollisions | STATE_Particle_FreezeTranslation | STATE_Particle_FreezeRotation,
    /** Delay collision updates to the particle				*/
    STATE_Particle_DelayCollisions = 0x40000000,
    /** Flag indicating the particle has had at least one collision	*/
    STATE_Particle_CollisionHasOccurred = 0x80000000,
    /** State mask. */
    STATE_Mask = 0xFE000000,
    /** Counter mask. */
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
    uint8* ParticleData; // this is also the memory block we allocated
    uint16* ParticleIndices; // not allocated, this is at the end of the memory block

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

#define DECLARE_PARTICLE_PTR(Name,Address)		\
	FBaseParticle* Name = (FBaseParticle*) (Address);
