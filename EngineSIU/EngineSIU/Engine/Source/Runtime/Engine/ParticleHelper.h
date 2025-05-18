#pragma once
#include "Define.h"

struct FParticleEmitterInstance;
struct FParticleMeshEmitterInstance;
struct FBaseParticle
{
    // 48 bytes
    FVector		OldLocation;			// Last frame's location, used for collision
    FVector		Location;				// Current location

    // 16 bytes
    FVector		BaseVelocity;			// Velocity = BaseVelocity at the start of each frame.
    float			Rotation;				// Rotation of particle (in Radians)

    // 16 bytes
    FVector		Velocity;				// Current velocity, gets reset to BaseVelocity each frame to allow 
    float			BaseRotationRate;		// Initial angular velocity of particle (in Radians per second)

    // 16 bytes
    FVector		BaseSize;				// Size = BaseSize at the start of each frame
    float			RotationRate;			// Current rotation rate, gets reset to BaseRotationRate each frame

    // 16 bytes
    FVector		Size;					// Current size, gets reset to BaseSize each frame
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

struct FMacroUVOverride
{
    FMacroUVOverride() : bOverride(false), Radius(0.f), Position(0.f, 0.f, 0.f) {}

    bool	bOverride;
    float   Radius;
    FVector Position;

    friend FORCEINLINE FArchive& operator<<(FArchive& Ar, FMacroUVOverride& O)
    {
        Ar << O.bOverride;
        Ar << O.Radius;
        Ar << O.Position;
        return Ar;
    }
};

struct FDynamicEmitterReplayDataBase
{
    /**	The type of emitter. */
    EDynamicEmitterType	eEmitterType;

    /**	The number of particles currently active in this emitter. */
    int32 ActiveParticleCount;

    int32 ParticleStride;
    FParticleDataContainer DataContainer;

    FVector Scale;

    /** Whether this emitter requires sorting as specified by artist.	*/
    int32 SortMode;

    /** MacroUV (override) data **/
    FMacroUVOverride MacroUVOverride;

    /** Constructor */
    FDynamicEmitterReplayDataBase()
        : eEmitterType(DET_Unknown),
        ActiveParticleCount(0),
        ParticleStride(0),
        Scale(FVector(1.0f)),
        SortMode(0)	// Default to PSORTMODE_None		  
    {
    }

    virtual ~FDynamicEmitterReplayDataBase()
    {
    }

    /** Serialization */
    virtual void Serialize(FArchive& Ar)
    {
        int32 EmitterTypeAsInt = eEmitterType;
        Ar << EmitterTypeAsInt;
        eEmitterType = static_cast<EDynamicEmitterType>(EmitterTypeAsInt);

        Ar << ActiveParticleCount;
        Ar << ParticleStride;

        TArray<uint8> ParticleData;
        TArray<uint16> ParticleIndices;

        //if (!Ar.IsLoading() && !Ar.IsObjectReferenceCollector())
        //{
        //    if (DataContainer.ParticleDataNumBytes)
        //    {
        //        ParticleData.AddUninitialized(DataContainer.ParticleDataNumBytes);
        //        FMemory::Memcpy(ParticleData.GetData(), DataContainer.ParticleData, DataContainer.ParticleDataNumBytes);
        //    }
        //    if (DataContainer.ParticleIndicesNumShorts)
        //    {
        //        ParticleIndices.AddUninitialized(DataContainer.ParticleIndicesNumShorts);
        //        FMemory::Memcpy(ParticleIndices.GetData(), DataContainer.ParticleIndices, DataContainer.ParticleIndicesNumShorts * sizeof(uint16));
        //    }
        //}

        Ar << ParticleData;
        Ar << ParticleIndices;

        //if (Ar.IsLoading())
        //{
        //    DataContainer.Free();
        //    if (ParticleData.Num())
        //    {
        //        DataContainer.Alloc(ParticleData.Num(), ParticleIndices.Num());
        //        FMemory::Memcpy(DataContainer.ParticleData, ParticleData.GetData(), DataContainer.ParticleDataNumBytes);
        //        if (DataContainer.ParticleIndicesNumShorts)
        //        {
        //            FMemory::Memcpy(DataContainer.ParticleIndices, ParticleIndices.GetData(), DataContainer.ParticleIndicesNumShorts * sizeof(uint16));
        //        }
        //    }
        //    else
        //    {
        //        check(!ParticleIndices.Num());
        //    }
        //}

        Ar << Scale;
        Ar << SortMode;
        Ar << MacroUVOverride;
    }

};


struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
    FDynamicSpriteEmitterReplayDataBase();
    ~FDynamicSpriteEmitterReplayDataBase();

    class UMaterialInterface* MaterialInterface;
    struct FParticleRequiredModule* RequiredModule;


    virtual void Serialize(FArchive& Ar);
};


struct FDynamicEmitterDataBase
{
    FDynamicEmitterDataBase(const class UParticleModuleRequired* RequiredModule);

    virtual ~FDynamicEmitterDataBase()
    {
    }

    /** Custom new/delete with recycling */
    void* operator new(size_t Size);
    void operator delete(void* RawMemory, size_t Size);

    /** Returns the source data for this particle system */
    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;

    /** Returns the current macro uv override. Specialized by FGPUSpriteDynamicEmitterData  */
    virtual const FMacroUVOverride& GetMacroUVOverride() const { return GetSource().MacroUVOverride; }

    /** Stat id of this object, 0 if nobody asked for it yet */
    //mutable TStatId StatID;
    /** true if this emitter is currently selected */
    uint32	bSelected : 1;
    /** true if this emitter has valid rendering data */
    uint32	bValid : 1;

    int32  EmitterIndex;
};

/**
 * The RHI's feature level indicates what level of support can be relied upon.
 * Note: these are named after graphics API's like ES3 but a feature level can be used with a different API (eg ERHIFeatureLevel::ES3.1 on D3D11)
 * As long as the graphics API supports all the features of the feature level (eg no ERHIFeatureLevel::SM5 on OpenGL ES3.1)
 */
namespace ERHIFeatureLevel
{
    enum Type : int
    {
        /** Feature level defined by the core capabilities of OpenGL ES2. Deprecated */
        ES2_REMOVED,

        /** Feature level defined by the core capabilities of OpenGL ES3.1 & Metal/Vulkan. */
        ES3_1,

        /**
         * Feature level defined by the capabilities of DX10 Shader Model 4.
         * SUPPORT FOR THIS FEATURE LEVEL HAS BEEN ENTIRELY REMOVED.
         */
        SM4_REMOVED,

        /**
         * Feature level defined by the capabilities of DX11 Shader Model 5.
         *   Compute shaders with shared memory, group sync, UAV writes, integer atomics
         *   Indirect drawing
         *   Pixel shaders with UAV writes
         *   Cubemap arrays
         *   Read-only depth or stencil views (eg read depth buffer as SRV while depth test and stencil write)
         * Tessellation is not considered part of Feature Level SM5 and has a separate capability flag.
         */
        SM5,

        /**
         * Feature level defined by the capabilities of DirectX 12 hardware feature level 12_2 with Shader Model 6.5
         *   Raytracing Tier 1.1
         *   Mesh and Amplification shaders
         *   Variable rate shading
         *   Sampler feedback
         *   Resource binding tier 3
         */
        SM6,

        Num
    };
};

struct FParticleOrder
{
    int32 ParticleIndex;

    union
    {
        float Z;
        uint32 C;
    };

    FParticleOrder(int32 InParticleIndex, float InZ) :
        ParticleIndex(InParticleIndex),
        Z(InZ)
    {
    }

    FParticleOrder(int32 InParticleIndex, uint32 InC) :
        ParticleIndex(InParticleIndex),
        C(InC)
    {
    }
};

struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{
    FDynamicSpriteEmitterDataBase(const UParticleModuleRequired* RequiredModule) :
        FDynamicEmitterDataBase(RequiredModule),
        bUsesDynamicParameter(false)
    {
    }

    virtual ~FDynamicSpriteEmitterDataBase()
    {
    }

    /**
 *	Get the vertex stride for the dynamic parameter rendering data
 */
    virtual int32 GetDynamicParameterVertexStride() const
    {
        //checkf(0, TEXT("GetDynamicParameterVertexStride MUST be overridden"));
        return 0;
    }

    /**
     *	Get the source replay data for this emitter
     */
    virtual const FDynamicSpriteEmitterReplayDataBase* GetSourceData() const
    {
        //checkf(0, TEXT("GetSourceData MUST be overridden"));
        return NULL;
    }

    /**
     *	Gets the information required for allocating this emitters indices from the global index array.
     */
    virtual void GetIndexAllocInfo(int32& OutNumIndices, int32& OutStride) const
    {
        //checkf(0, TEXT("GetIndexAllocInfo is not valid for this class."));
    }

    void SortSpriteParticles(int32 SortMode, bool bLocalSpace,
        int32 ParticleCount, const uint8* ParticleData, int32 ParticleStride, const uint16* ParticleIndices,
        const class FSceneView* View, const FMatrix& LocalToWorld, FParticleOrder* ParticleOrder) const;

    virtual int32 GetDynamicVertexStride(ERHIFeatureLevel::Type /*InFeatureLevel*/) const = 0;

    /** true if the particle emitter utilizes the DynamicParameter module */
    uint32 bUsesDynamicParameter : 1;
};


/**
 * Per-particle data sent to the GPU.
 */
struct FParticleSpriteVertex
{
    /** The position of the particle. */
    FVector Position;
    /** The relative time of the particle. */
    float RelativeTime;
    /** The previous position of the particle. */
    FVector	OldPosition;
    /** Value that remains constant over the lifetime of a particle. */
    float ParticleId;
    /** The size of the particle. */
    FVector2D Size;
    /** The rotation of the particle. */
    float Rotation;
    /** The sub-image index for the particle. */
    float SubImageIndex;
    /** The color of the particle. */
    FLinearColor Color;

    FVector2D UV;
};

//	FParticleSpriteVertexDynamicParameter
struct FParticleVertexDynamicParameter
{
    /** The dynamic parameter of the particle			*/
    float			DynamicValue[4];
};

struct FDynamicSpriteEmitterReplayData
    : public FDynamicSpriteEmitterReplayDataBase
{
    /** Constructor */
    FDynamicSpriteEmitterReplayData()
    {
    }


    /** Serialization */
    virtual void Serialize(FArchive& Ar)
    {
        // Call parent implementation
        FDynamicSpriteEmitterReplayDataBase::Serialize(Ar);

        // ...
    }

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

    /** Initialize this emitter's dynamic rendering data, called after source data has been filled in */
    void Init(bool bInSelected);

    /**
    *	Get the vertex stride for the dynamic rendering data
    */
    virtual int32 GetDynamicVertexStride(ERHIFeatureLevel::Type InFeatureLevel) const override
    {
        return sizeof(FParticleSpriteVertex);
    }

    virtual int32 GetDynamicParameterVertexStride() const override
    {
        return sizeof(FParticleVertexDynamicParameter);
    }

    /**
    *	Get the source replay data for this emitter
    */
    virtual const FDynamicSpriteEmitterReplayDataBase* GetSourceData() const override
    {
        return &Source;
    }

    /** Returns the source data for this particle system */
    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    /** The frame source data for this particle system.  This is everything needed to represent this
        this particle system frame.  It does not include any transient rendering thread data.  Also, for
        non-simulating 'replay' particle systems, this data may have come straight from disk! */
    FDynamicSpriteEmitterReplayData Source;

    /** Uniform parameters. Most fields are filled in when updates are sent to the rendering thread, some are per-view! */
    //FParticleSpriteUniformParameters UniformParameters;
};

// Per-particle data sent to the GPU.
struct FMeshParticleInstanceVertex
{
    /** The color of the particle. */
    FLinearColor Color;

    /** The instance to world transform of the particle. Translation vector is packed into W components. */
    FVector4 Transform[3];

    /** The velocity of the particle, XYZ: direction, W: speed. */
    FVector4 Velocity;

    /** The sub-image texture offsets for the particle. */
    int16 SubUVParams[4];

    /** The sub-image lerp value for the particle. */
    float SubUVLerp;

    /** The relative time of the particle. */
    float RelativeTime;
};

struct FMeshParticleInstanceVertexDynamicParameter
{
    /** The dynamic parameter of the particle. */
    float DynamicValue[4];
};


struct FDynamicMeshEmitterReplayData
    : public FDynamicSpriteEmitterReplayDataBase
{
    int32	SubUVInterpMethod;
    int32	SubUVDataOffset;
    int32	SubImages_Horizontal;
    int32	SubImages_Vertical;
    bool	bScaleUV;
    int32	MeshRotationOffset;
    int32	MeshMotionBlurOffset;
    uint8	MeshAlignment;
    bool	bMeshRotationActive;
    FVector	LockedAxis;

    /** Constructor */
    FDynamicMeshEmitterReplayData() :
        SubUVInterpMethod(0),
        SubUVDataOffset(0),
        SubImages_Horizontal(0),
        SubImages_Vertical(0),
        bScaleUV(false),
        MeshRotationOffset(0),
        MeshMotionBlurOffset(0),
        MeshAlignment(0),
        bMeshRotationActive(false),
        LockedAxis(1.0f, 0.0f, 0.0f)
    {
    }


    /** Serialization */
    virtual void Serialize(FArchive& Ar)
    {
        // Call parent implementation
        FDynamicSpriteEmitterReplayDataBase::Serialize(Ar);

        Ar << SubUVInterpMethod;
        Ar << SubUVDataOffset;
        Ar << SubImages_Horizontal;
        Ar << SubImages_Vertical;
        Ar << bScaleUV;
        Ar << MeshRotationOffset;
        Ar << MeshMotionBlurOffset;
        Ar << MeshAlignment;
        Ar << bMeshRotationActive;
        Ar << LockedAxis;
    }

};

struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterData
{
    FDynamicMeshEmitterData(const UParticleModuleRequired* RequiredModule);

    virtual ~FDynamicMeshEmitterData();

    void Init(bool bInSelected,
        const FParticleMeshEmitterInstance* InEmitterInstance,
        class UStaticMesh* InStaticMesh,
        bool InUseStaticMeshLODs,
        float InLODSizeScale,
        ERHIFeatureLevel::Type InFeatureLevel);

    virtual int32 GetDynamicVertexStride(ERHIFeatureLevel::Type /*InFeatureLevel*/) const override
    {
        return sizeof(FMeshParticleInstanceVertex);
    }

    virtual int32 GetDynamicParameterVertexStride() const override
    {
        return sizeof(FMeshParticleInstanceVertexDynamicParameter);
    }
    virtual const FDynamicSpriteEmitterReplayDataBase* GetSourceData() const override
    {
        return &Source;
    }

    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    FDynamicMeshEmitterReplayData Source;
    int32					LastFramePreRendered;

    UStaticMesh* StaticMesh;

    const FParticleMeshEmitterInstance* EmitterInstance;
};
