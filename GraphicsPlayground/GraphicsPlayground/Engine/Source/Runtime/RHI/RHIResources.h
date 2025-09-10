#pragma once

#include <atomic>

#include "HAL/PlatformType.h"
#include "Container/Array.h"
#include "Container/Queue.h"
#include "CoreUObject/UObject/NameTypes.h"
#include "RHI.h"
#include "PixelFormat.h"
#include "RHIDefinitions.h"
#include "RHIAccess.h"


struct FRHIResource
{
public:
    FRHIResource(ERHIResourceType InResourceType) : ResourceType(InResourceType) {}
protected:
    friend class FDynamicRHI;
    friend class FRHICommandListExecutor;

    /** Only called by classes above */
    virtual ~FRHIResource() {}

public:
    /** 
     * FRefCountPtr에 따르면,
     * AddRef() (=constructor) -> 리소스 사용 시작,
     * 리소스 사용 끝 -> Release() (=destructor)의 순서로 이루어져야한다.
     * 따라서 AddRef()에서 memory_order_acquire를 사용해서 동적 파이프라이닝에 의해서
     * 리소스 사용 시작이 AddRef()뒤에 오도록 보장한다. Release는 그 반대
     */
    FORCEINLINE uint32 AddRef() const
    {
        int32 NewValue = AtomicFlags.AddRef(std::memory_order_acquire);
        assert(NewValue > 0);
        return NewValue;
    }

    FORCEINLINE uint32 Release()
    {
        int32 NewValue = AtomicFlags.Release(std::memory_order_release);
        assert(NewValue >= 0);
        if ( NewValue == 0)
        {
            MarkForDelete();
        }
        return NewValue;
    }

    FORCEINLINE uint32 GetRefCount() const
    {
        return AtomicFlags.GetNumRefs(std::memory_order_relaxed);
    }

private:
	void MarkForDelete();

    /** Use 32bit to pack MarkedForDelete flag, Deleting flag, and refcount*/
	class FAtomicFlags
	{
		static constexpr uint32 MarkedForDeleteBit    = 1 << 30;
		static constexpr uint32 DeletingBit           = 1 << 31;
		static constexpr uint32 NumRefsMask           = ~(MarkedForDeleteBit | DeletingBit);

		std::atomic_uint Packed = { 0 };

	public:
		int32 AddRef(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_add(1, MemoryOrder);
			assert((OldPacked & DeletingBit) == 0); // Resource is being deleted.
			int32  NumRefs = (OldPacked & NumRefsMask) + 1;
			assert(NumRefs < NumRefsMask); // Reference count has overflowed.
			return NumRefs;
		}

		int32 Release(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_sub(1, MemoryOrder);
			assert((OldPacked & DeletingBit) == 0); // Resource is being deleted.
			int32  NumRefs = (OldPacked & NumRefsMask) - 1;
			assert(NumRefs >= 0); // Reference count has underflowed.
			return NumRefs;
		}

		bool MarkForDelete(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_or(MarkedForDeleteBit, MemoryOrder);
			assert((OldPacked & DeletingBit) == 0);
			return (OldPacked & MarkedForDeleteBit) != 0;
		}

		bool UnmarkForDelete(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_xor(MarkedForDeleteBit, MemoryOrder);
			assert((OldPacked & DeletingBit) == 0);
			bool  OldMarkedForDelete = (OldPacked & MarkedForDeleteBit) != 0;
			assert(OldMarkedForDelete == true);
			return OldMarkedForDelete;
		}

		bool IsDeleting()
		{
			uint32 LocalPacked = Packed.load(std::memory_order_acquire);
			assert((LocalPacked & MarkedForDeleteBit) != 0);
			assert((LocalPacked & DeletingBit) == 0);
			uint32 NumRefs = LocalPacked & NumRefsMask;

			if (NumRefs == 0) // caches can bring dead objects back to life
			{
				Packed.fetch_or(DeletingBit, std::memory_order_acquire);
				return true;
			}
			else
			{
				UnmarkForDelete(std::memory_order_release);
				return false;
			}
		}

		bool IsMarkedForDelete(std::memory_order MemoryOrder)
		{
			return (Packed.load(MemoryOrder) & MarkedForDeleteBit) != 0;
		}

		int32 GetNumRefs(std::memory_order MemoryOrder)
		{
			return Packed.load(MemoryOrder) & NumRefsMask;
		}
	};
	mutable FAtomicFlags AtomicFlags;

    const ERHIResourceType ResourceType;
};

/** Which to clear in the target texture */
enum class EClearBinding
{
    ENoneBound,
    EColorBound,
    EDepthStencilBound
};

/** Values for clear texture */
struct FClearValueBinding
{
    float Depth;
    uint32 Stencil;
    float Color[4];
};

class FExclusiveDepthStencil
{
public:
	enum Type : uint8
	{
		// don't use those directly, use the combined versions below
		// 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
		DepthNop = 0x00,
		DepthRead = 0x01,
		DepthWrite = 0x02,
		DepthMask = 0x0f,
		StencilNop = 0x00,
		StencilRead = 0x10,
		StencilWrite = 0x20,
		StencilMask = 0xf0,

		// use those:
		DepthNop_StencilNop = DepthNop + StencilNop,
		DepthRead_StencilNop = DepthRead + StencilNop,
		DepthWrite_StencilNop = DepthWrite + StencilNop,
		DepthNop_StencilRead = DepthNop + StencilRead,
		DepthRead_StencilRead = DepthRead + StencilRead,
		DepthWrite_StencilRead = DepthWrite + StencilRead,
		DepthNop_StencilWrite = DepthNop + StencilWrite,
		DepthRead_StencilWrite = DepthRead + StencilWrite,
		DepthWrite_StencilWrite = DepthWrite + StencilWrite,
	};

private:
	Type Value;

public:
	// constructor
	FExclusiveDepthStencil(Type InValue = DepthNop_StencilNop)
		: Value(InValue)
	{
	}

    inline bool IsUsingDepthStencil() const
	{
		return Value != DepthNop_StencilNop;
	}
	inline bool IsUsingDepth() const
	{
		return (ExtractDepth() != DepthNop);
	}
	inline bool IsUsingStencil() const
	{
		return (ExtractStencil() != StencilNop);
	}
	inline bool IsDepthWrite() const
	{
		return ExtractDepth() == DepthWrite;
	}
	inline bool IsDepthRead() const
	{
		return ExtractDepth() == DepthRead;
	}
	inline bool IsStencilWrite() const
	{
		return ExtractStencil() == StencilWrite;
	}
	inline bool IsStencilRead() const
	{
		return ExtractStencil() == StencilRead;
	}

	inline bool IsAnyWrite() const
	{
		return IsDepthWrite() || IsStencilWrite();
	}

	inline void SetDepthWrite()
	{
		Value = (Type)(ExtractStencil() | DepthWrite);
	}
	inline void SetStencilWrite()
	{
		Value = (Type)(ExtractDepth() | StencilWrite);
	}
	inline void SetDepthStencilWrite(bool bDepth, bool bStencil)
	{
		Value = DepthNop_StencilNop;

		if (bDepth)
		{
			SetDepthWrite();
		}
		if (bStencil)
		{
			SetStencilWrite();
		}
	}
	bool operator==(const FExclusiveDepthStencil& rhs) const
	{
		return Value == rhs.Value;
	}

	bool operator != (const FExclusiveDepthStencil& RHS) const
	{
		return Value != RHS.Value;
	}

	inline bool IsValid(FExclusiveDepthStencil& Current) const
	{
		Type Depth = ExtractDepth();

		if (Depth != DepthNop && Depth != Current.ExtractDepth())
		{
			return false;
		}

		Type Stencil = ExtractStencil();

		if (Stencil != StencilNop && Stencil != Current.ExtractStencil())
		{
			return false;
		}

		return true;
	}

	inline void GetAccess(ERHIAccess& DepthAccess, ERHIAccess& StencilAccess) const
	{
		DepthAccess = ERHIAccess::None;

		// SRV access is allowed whilst a depth stencil target is "readable".
		constexpr ERHIAccess DSVReadOnlyMask =
			ERHIAccess::DSVRead;

		// If write access is required, only the depth block can access the resource.
		constexpr ERHIAccess DSVReadWriteMask =
			ERHIAccess::DSVRead |
			ERHIAccess::DSVWrite;

		if (IsUsingDepth())
		{
			DepthAccess = IsDepthWrite() ? DSVReadWriteMask : DSVReadOnlyMask;
		}

		StencilAccess = ERHIAccess::None;

		if (IsUsingStencil())
		{
			StencilAccess = IsStencilWrite() ? DSVReadWriteMask : DSVReadOnlyMask;
		}
	}

private:
	inline Type ExtractDepth() const
	{
		return (Type)(Value & DepthMask);
	}
	inline Type ExtractStencil() const
	{
		return (Type)(Value & StencilMask);
	}
};

// -----------------------------------------------------------------------------
// States
// -----------------------------------------------------------------------------

class FRHISamplerState : public FRHIResource 
{
public:
	FRHISamplerState() : FRHIResource(RRT_SamplerState) {}
    virtual bool GetInitializer(struct FSamplerStateInitializerRHI& Init) { return false; }
};

class FRHIRasterizerState : public FRHIResource
{
public:
	FRHIRasterizerState() : FRHIResource(RRT_RasterizerState) {}
	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) { return false; }
};

class FRHIDepthStencilState : public FRHIResource
{
public:
	FRHIDepthStencilState() : FRHIResource(RRT_DepthStencilState) {}
	virtual bool GetInitializer(struct FDepthStencilStateInitializerRHI& Init) { return false; }
};

class FRHIBlendState : public FRHIResource
{
public:
	FRHIBlendState() : FRHIResource(RRT_BlendState) {}
	virtual bool GetInitializer(class FBlendStateInitializerRHI& Init) { return false; }
};

// -----------------------------------------------------------------------------
// Shader bindings (vertex, index, uniform)
// -----------------------------------------------------------------------------

/** vertex layout */
struct FRHIVertexDeclaration : public FRHIResource
{
    FRHIVertexDeclaration() : FRHIResource(RRT_VertexDeclaration) {}
    virtual bool GetInitializer(const FVertexDeclarationElementList& Init) { return false; }
};

/** 
 * A base class for bindings over one pipeline. For legacy APIs which does not have pipeline state.
 * Implementation (variables) should be done in the API.
 * Contains input layout and shaders of the pipeline
 */
struct FRHIBoundShaderState : public FRHIResource
{
    FRHIBoundShaderState() : FRHIResource(RRT_BoundShaderState) {}
};

/** Shader layout is generated following this struct */
struct FShaderResourceTable
{
	/** Mapping of bound SRVs to their location in resource tables. */
	TArray<uint32> ShaderResourceViewMap;

	/** Mapping of bound sampler states to their location in resource tables. */
	TArray<uint32> SamplerMap;

	/** Mapping of bound UAVs to their location in resource tables. */
	TArray<uint32> UnorderedAccessViewMap;
};

struct FRHIShaderData
{
    FString EntryPoint;
    TArray<FString> Defines;
    FString FilePath;

    FShaderResourceTable ShaderResourceTable;
	TArray<FUniformBufferStaticSlot> StaticSlots; // ?? 뭐하는애지
};

struct FRHIShader : public FRHIResource, public FRHIShaderData
{
    FRHIShader(ERHIResourceType InResourceType, EShaderType InShaderType)
        : FRHIResource(InResourceType), ShaderType(InShaderType)
    {
    }

    // we does not use shader frequency(type)
    EShaderType ShaderType;
};

class FRHIGraphicsShader : public FRHIShader
{
public:
    explicit FRHIGraphicsShader(ERHIResourceType InResourceType, EShaderType InShaderType)
        : FRHIShader(InResourceType, InShaderType) {}
};

struct FRHIVertexShader : public FRHIGraphicsShader
{
    FRHIVertexShader() : FRHIGraphicsShader(RRT_VertexShader, EShaderType::Vertex) {}
};

struct FRHIPixelShader : public FRHIGraphicsShader
{
    FRHIPixelShader() : FRHIGraphicsShader(RRT_PixelShader, EShaderType::Pixel) {}
};

struct FRHIGeometryShader : public FRHIGraphicsShader
{
    FRHIGeometryShader() : FRHIGraphicsShader(RRT_GeometryShader, EShaderType::Geometry) {}
};

struct FRHIComputeShader : public FRHIGraphicsShader
{
    FRHIComputeShader() : FRHIGraphicsShader(RRT_ComputeShader, EShaderType::Compute) {}
};

/** For latest graphics APIs only. */
// struct FRHIGraphicsPipelineState : public FRHIResource
// {
//     FRHIGraphicsPipelineState() : FRHIResource(RRT_GraphicsPipelineState) {}
// };

// -----------------------------------------------------------------------------
// Uniform Buffer
// -----------------------------------------------------------------------------

/** Data structure in uniform buffer and parameters */
struct FRHIUniformBufferResource
{
    /** Byte offset to each resource in the uniform buffer memory. */
	uint16 MemberOffset;

	/** Type of the member that allow (). */
	EUniformBufferBaseType MemberType;
};

struct FRHIUniformBufferResourceInitializer
{
    uint16 MemberOffset = 0;
    EUniformBufferBaseType MemberType;
};

struct FRHIUniformBufferLayoutInitializer
{
    /** uniform/constant buffer */
    TArray<FRHIUniformBufferResourceInitializer> Resources;
};

struct FRHIUniformBufferLayout : public FRHIResource
{
    FRHIUniformBufferLayout(FRHIUniformBufferLayoutInitializer InInitializer)
        : FRHIResource(RRT_UniformBufferLayout)
    {
        Resources.SetNum(InInitializer.Resources.Num());
        for (int32 Index = 0; Index < InInitializer.Resources.Num(); ++Index)
        {
            const FRHIUniformBufferResourceInitializer& Initializer = InInitializer.Resources[Index];
            Resources[Index] = { Initializer.MemberOffset, Initializer.MemberType };
        }
    }

    TArray<FRHIUniformBufferResource> Resources;
};

struct FRHIUniformBuffer : public FRHIResource
{
    /** To avoid duplicated layouts, we take pointer */
    FRHIUniformBuffer(const FRHIUniformBufferLayout* InLayout)
        : FRHIResource(RRT_UniformBuffer)
        , Layout(InLayout)
    {
    }

    TArray<std::shared_ptr<FRHIResource>> ResourceTable; // ???

    std::shared_ptr<const FRHIUniformBufferLayout> Layout;
};

// -----------------------------------------------------------------------------
// Buffer
// -----------------------------------------------------------------------------

struct FRHIViewableResource : public FRHIResource
{
    FRHIViewableResource(ERHIResourceType InResourceType, ERHIAccess InAccess)
        : FRHIResource(InResourceType), Access(InAccess) {}

    /** Access flags for this resource */
    ERHIAccess Access;
};

struct FRHIBufferDesc
{
	FRHIBufferDesc() = default;
	FRHIBufferDesc(uint32 InSize, uint16 InStride, EBufferUsageFlags InUsage)
		: Size(InSize)
		, Stride(InStride)
		, Usage(InUsage)
	{
	}

    /** Size of the buffer memory in bytes */
    uint32 Size = 0;

    /** Stride between elements */
    uint16 Stride = 0;

    /** Usage flags for this buffer */
    EBufferUsageFlags Usage = EBufferUsageFlags::None;
};

#pragma region FRHIBufferCreateDesc
// /** Helper functions and name for debug */
// struct FRHIBufferCreateDesc : public FRHIBufferDesc
// {
// 	static FRHIBufferCreateDesc Create(const TCHAR* InDebugName, EBufferUsageFlags InUsage)
// 	{
// 		return FRHIBufferCreateDesc(InDebugName, InUsage);
// 	}

// 	static FRHIBufferCreateDesc Create(const TCHAR* InDebugName, uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage)
// 	{
// 		return FRHIBufferCreateDesc(InDebugName, InSize, InStride, InUsage);
// 	}
	
// 	static FRHIBufferCreateDesc Create(const TCHAR* InDebugName, const FRHIBufferDesc& InDesc)
// 	{
// 		return FRHIBufferCreateDesc(InDebugName, InDesc);
// 	}

// 	static FRHIBufferCreateDesc CreateVertex(const TCHAR* InDebugName)
// 	{
// 		return Create(InDebugName, EBufferUsageFlags::VertexBuffer);
// 	}

// 	static FRHIBufferCreateDesc CreateVertex(const TCHAR* InDebugName, uint32 InSize)
// 	{
// 		return Create(InDebugName, InSize, 0, EBufferUsageFlags::VertexBuffer);
// 	}

// 	template<typename TVertexType>
// 	static FRHIBufferCreateDesc CreateVertex(const TCHAR* InDebugName, uint32 InCount)
// 	{
// 		return Create(InDebugName, InCount * sizeof(TVertexType), 0, EBufferUsageFlags::VertexBuffer);
// 	}

// 	static FRHIBufferCreateDesc CreateIndex(const TCHAR* InDebugName)
// 	{
// 		return Create(InDebugName, EBufferUsageFlags::IndexBuffer);
// 	}

// 	static FRHIBufferCreateDesc CreateIndex(const TCHAR* InDebugName, uint32 InSize, uint32 InStride)
// 	{
// 		return Create(InDebugName, InSize, InStride, EBufferUsageFlags::IndexBuffer);
// 	}

// 	template<typename TIndexType>
// 	static FRHIBufferCreateDesc CreateIndex(const TCHAR* InDebugName, uint32 InCount)
// 	{
// 		return Create(InDebugName, InCount * sizeof(TIndexType), sizeof(TIndexType), EBufferUsageFlags::IndexBuffer);
// 	}

// 	static FRHIBufferCreateDesc CreateStructured(const TCHAR* InDebugName)
// 	{
// 		return Create(InDebugName, EBufferUsageFlags::StructuredBuffer);
// 	}

// 	static FRHIBufferCreateDesc CreateStructured(const TCHAR* InDebugName, uint32 InSize, uint32 InStride)
// 	{
// 		return Create(InDebugName, InSize, InStride, EBufferUsageFlags::StructuredBuffer);
// 	}

// 	template<typename TStructureType>
// 	static FRHIBufferCreateDesc CreateStructured(const TCHAR* InDebugName, uint32 InCount)
// 	{
// 		return Create(InDebugName, InCount * sizeof(TStructureType), sizeof(TStructureType), EBufferUsageFlags::StructuredBuffer);
// 	}

// 	static FRHIBufferCreateDesc CreateUniform(const TCHAR* InDebugName)
// 	{
// 		return Create(InDebugName, EBufferUsageFlags::UniformBuffer);
// 	}

// 	FRHIBufferCreateDesc() = default;

// 	FRHIBufferCreateDesc(const TCHAR* InDebugName, EBufferUsageFlags InUsage)
// 		: DebugName(InDebugName)
// 	{
// 		Usage = InUsage;
// 	}

// 	FRHIBufferCreateDesc(const TCHAR* InDebugName, uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage)
// 		: FRHIBufferDesc(InSize, InStride, InUsage)
// 		, DebugName(InDebugName)
// 	{
// 	}

// 	FRHIBufferCreateDesc(const TCHAR* InDebugName, const FRHIBufferDesc& InOtherDesc)
// 		: FRHIBufferDesc(InOtherDesc)
// 		, DebugName(InDebugName)
// 	{
// 	}

//     void SetInitialState(ERHIAccess InInitialState)
//     {
//         InitialState = InInitialState;
//     }

//     ERHIAccess InitialState = ERHIAccess::Unknown;
//     const TCHAR* DebugName;
// };
#pragma endregion FRHIBufferCreateDesc

struct FRHIBuffer : public FRHIViewableResource
{
public:
    FRHIBuffer(FRHIBufferDesc InCreateDesc)
    : FRHIViewableResource(RRT_Buffer, ERHIAccess::Unknown)
    , Desc(InCreateDesc)
    {}

	const FRHIBufferDesc& GetDesc() const
	{
		return Desc;
	}

	uint32 GetSize() const
	{
		return Desc.Size;
	}

	uint32 GetStride() const
	{
		return Desc.Stride;
	}

	EBufferUsageFlags GetUsage() const
	{
		return Desc.Usage;
	}

private:
    FRHIBufferDesc Desc;
};

/** 
 * Simple pointer to vertex buffer.
 * @todo : redundant shared pointers
 */
class FRHIStreamSourceSlot : public FRHIResource
{
	friend class FRHICommandListBase;
	friend class FRHICommandList;
public:
	static TRefCountPtr<FRHIStreamSourceSlot> Create(FRHIBuffer* InBuffer)
	{
		return new FRHIStreamSourceSlot(InBuffer);
	}

private:
	FRHIStreamSourceSlot(FRHIBuffer* InBuffer)
		: FRHIResource(RRT_StreamSourceSlot)
		, Buffer(InBuffer)
	{}

	TRefCountPtr<FRHIBuffer> Buffer;
};

// -----------------------------------------------------------------------------
// Texture / Sampler
// -----------------------------------------------------------------------------

/** Descriptor used to create a texture resource */
struct FRHITextureDesc
{
	FRHITextureDesc() = default;

	FRHITextureDesc(const FRHITextureDesc& Other)
	{
		*this = Other;
	}

	FRHITextureDesc(ETextureDimension InDimension)
		: Dimension(InDimension)
	{}

	FRHITextureDesc(
		  ETextureDimension   InDimension
		, ETextureCreateFlags InFlags
		, EPixelFormat        InFormat
        , FClearValueBinding  InClearColor
        , uint16              InDimX
        , uint16              InDimY
        , uint16              InDimZ
		, uint16              InArraySize
		, uint8               InNumMips
		, uint8               InNumSamples
		)
		: Dimension (InDimension )
		, Flags     (InFlags     )
		, Format    (InFormat    )
        , ClearValue(InClearColor)
        , DimX      (InDimX      )
        , DimY      (InDimY      )
        , DimZ      (InDimZ      )
		, ArraySize (InArraySize )
		, NumMips   (InNumMips   )
		, NumSamples(InNumSamples)
	{}

	/** Texture flags passed on to RHI texture. */
	ETextureCreateFlags Flags = ETextureCreateFlags::None;

    /** Dimension of the texture */
    uint16 DimX = 1, DimY = 1, DimZ = 1;

	/** The number of array elements in the texture. (Keep at 1 if dimension is 3D). */
	uint16 ArraySize = 1;

	/** Number of mips in the texture mip-map chain. */
	uint8 NumMips = 1;

	/** Number of samples in the texture. >1 for MSAA. */
	uint8 NumSamples = 1;
    
    /** Values used to clear this texture. */
    FClearValueBinding ClearValue;

	/** Texture dimension to use when creating the RHI texture. */
	ETextureDimension Dimension = ETextureDimension::Texture2D;

	/** Pixel format used to create RHI texture. */
	EPixelFormat Format = PF_Unknown;

	/** Texture format used when creating the UAV. PF_Unknown means to use the default one (same as Format). */
	EPixelFormat UAVFormat = PF_Unknown;
};

extern ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage);
extern ERHIAccess RHIGetDefaultResourceState(EBufferUsageFlags InUsage/*, bool bInHasInitialData*/);

struct FRHITextureCreateDesc : public FRHITextureDesc
{
	static FRHITextureCreateDesc Create(const TCHAR* InDebugName, ETextureDimension InDimension)
	{
		return FRHITextureCreateDesc(InDebugName, InDimension);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2D);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2DArray);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture3D);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCube);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCubeArray);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(SizeX, SizeY, 1)
			.SetFormat(Format);
	}
    
	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(SizeX, SizeY, 1)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 SizeZ, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(SizeX, SizeY, SizeZ)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* DebugName, uint32 Size, EPixelFormat Format)
	{
		return CreateCube(DebugName)
			.SetExtent(Size, Size, 1)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* DebugName, uint32 Size, uint16 ArraySize, EPixelFormat Format)
	{
		return CreateCubeArray(DebugName)
			.SetExtent(Size, Size, 1)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	FRHITextureCreateDesc() = default;

	// Constructor with minimal argument set. Name and dimension are always required.
	FRHITextureCreateDesc(const TCHAR* InDebugName, ETextureDimension InDimension)
		: FRHITextureDesc(InDimension) 
		, DebugName(InDebugName)
	{
	}

    FRHITextureCreateDesc& SetExtent(uint16 InX, uint16 InY, uint16 InZ)
    {
        DimX = InX;
        DimY = InY;
        DimZ = InZ;
        return *this;
    }
	FRHITextureCreateDesc& SetArraySize(uint16 InArraySize)                    { ArraySize = InArraySize;                  return *this; }
	FRHITextureCreateDesc& SetNumMips(uint8 InNumMips)                         { NumMips = InNumMips;                      return *this; }
	FRHITextureCreateDesc& SetNumSamples(uint8 InNumSamples)                   { NumSamples = InNumSamples;                return *this; }
	FRHITextureCreateDesc& SetDimension(ETextureDimension InDimension)         { Dimension = InDimension;                  return *this; }
	FRHITextureCreateDesc& SetFormat(EPixelFormat InFormat)                    { Format = InFormat;                        return *this; }
	FRHITextureCreateDesc& SetUAVFormat(EPixelFormat InUAVFormat)              { UAVFormat = InUAVFormat;                  return *this; }
	FRHITextureCreateDesc& SetInitialState(ERHIAccess InInitialState)          { InitialState = InInitialState;            return *this; }
	FRHITextureCreateDesc& SetDebugName(const TCHAR* InDebugName)              { DebugName = InDebugName;                  return *this; }

	/* The RHI access state that the resource will be created in. */
	ERHIAccess InitialState = ERHIAccess::Unknown;

	/* A friendly name for the resource. */
	const TCHAR* DebugName = nullptr;

	FName ClassName = NAME_None;	// The owner class of FRHITexture used for Insight asset metadata tracing
	FName OwnerName = NAME_None;	// The owner name used for Insight asset metadata tracing
};

class FRHITexture : public FRHIViewableResource
{
protected:
	/** Initialization constructor. Should only be called by platform RHI implementations. */
	FRHITexture(const FRHITextureCreateDesc& InDesc)
        : FRHIViewableResource(RRT_Texture, InDesc.InitialState)
        , TextureDesc(InDesc)
    {
    }

public:
    /** Desc used to create this texture */
	virtual const FRHITextureDesc& GetDesc() const { return TextureDesc; }
	
	///
	/// Virtual functions implemented per RHI
	///  
    /** API specific texture buffer */
	virtual void* GetNativeResource() const
	{
		return nullptr;
	}

    /** API specific shader resource view */
	virtual void* GetNativeShaderResourceView() const
	{
		return nullptr;
	}

    /** Returns API specific base class */
	virtual void* GetTextureBaseRHI()
	{
		return nullptr;
	}

private:
	FRHITextureDesc TextureDesc;
};

// -----------------------------------------------------------------------------
// Viewport (SwapChain)
// -----------------------------------------------------------------------------

/** 
 * Viewport is highly API-dependent, so FRHIViewport is kind of an abstract class.
 * Derived classes will implement the platform-specific details.
 * The derived class will be used as just a "struct",
 * while the class derives from FDynamicRHI will execute the platform-specific
 * functionality (like presenting the back buffer)
 */
class FRHIViewport : public FRHIResource
{
public:
    FRHIViewport() : FRHIResource(RRT_Viewport) {}

    /** Ticks the viewport on the game thread */
    virtual void Tick(float DeltaTime) {}
    virtual void WaitForFrameEventCompletion() {}
    virtual void IssueFrameEvent() {}
};

// -----------------------------------------------------------------------------
// Views
// -----------------------------------------------------------------------------

struct FRHIRange16
{
    uint16 First;
    uint16 Num;
};

struct FRHIRange8
{
    uint8 First;
    uint8 Num;
};

/** 
 * The unified RHI view descriptor. The descriptor is stored in FRHIView, the base class of all RHI views.
 * GetViewInfo() returns FViewInfo, which each API creates its own implementation for.
 */
struct FRHIViewDesc
{
	enum class EViewType : uint8
	{
		BufferSRV   = 0,
		BufferUAV   = 1,
		TextureSRV  = 2,
		TextureUAV  = 3,

        NumBits     = 2
	};

	enum class EBufferType : uint8
	{
		Unknown               = 0,

		Typed                 = 1,
		Structured            = 2,
		Raw                   = 3,

        NumBits               = 2
	};

	enum class EDimension : uint8
	{
		Unknown          = 0,

		Texture2D        = 1,
		Texture2DArray   = 2,
		TextureCube      = 3,
		TextureCubeArray = 4,
		Texture3D        = 5,

		NumBits          = 3
	};

	// Properties that apply to all views.
	struct FCommon
	{
        EViewType    ViewType;
		EPixelFormat Format;
	};

    /** Properties for buffers, not texture */
	struct FBuffer : public FCommon
	{
		EBufferType BufferType;
		uint32      OffsetInBytes;
        uint32      NumElements;
        uint32      Stride;

		struct FViewInfo;
	protected:
		FViewInfo GetViewInfo(FRHIBuffer* TargetBuffer) const;
	};

    /**
     * Properties for textures.
     * FInitializer and FViewInfo for each struct is implemented below.
     * Use FViewInfo when creating the view with API implementations.
     * The fields does not need to correspond to actual texture properties.
     */
	struct FTexture : public FCommon
	{
		uint8            bDisableSRGB;
		EDimension       Dimension;
        FRHIRange8       MipRange;
        FRHIRange16      ArrayRange;

		struct FViewInfo;
	protected:
		FViewInfo GetViewInfo(FRHITexture* TargetTexture) const;
	};

	struct FBufferSRV : public FBuffer
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHIBuffer* TargetBuffer) const;
	};

	struct FBufferUAV : public FBuffer
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHIBuffer* TargetBuffer) const;
	};

	struct FTextureSRV : public FTexture
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHITexture* TargetTexture) const;
	};

	struct FTextureUAV : public FTexture
	{
		struct FInitializer;
		struct FViewInfo;
		FViewInfo GetViewInfo(FRHITexture* TargetTexture) const;
	};

	union
	{
		FCommon Common;
		union
		{
			FBufferSRV SRV;
			FBufferUAV UAV;
		} Buffer;
		union
		{
			FTextureSRV SRV;
			FTextureUAV UAV;
		} Texture;
	};

	static inline FBufferSRV::FInitializer CreateBufferSRV();
	static inline FBufferUAV::FInitializer CreateBufferUAV();

	static inline FTextureSRV::FInitializer CreateTextureSRV();
	static inline FTextureUAV::FInitializer CreateTextureUAV();

	bool IsSRV() const { return Common.ViewType == EViewType::BufferSRV || Common.ViewType == EViewType::TextureSRV; }
	bool IsUAV() const { return !IsSRV(); }

	bool IsBuffer () const { return Common.ViewType == EViewType::BufferSRV || Common.ViewType == EViewType::BufferUAV; }
	bool IsTexture() const { return !IsBuffer(); }

	bool operator == (FRHIViewDesc const& RHS) const
	{
		return memcmp(this, &RHS, sizeof(*this)) == 0;
	}

	bool operator != (FRHIViewDesc const& RHS) const
	{
		return !(*this == RHS);
	}

	FRHIViewDesc()
		: FRHIViewDesc(EViewType::BufferSRV)
	{
		memset(this, 0, sizeof(*this));
	}

protected:
    /** Used by helper functions */
	FRHIViewDesc(EViewType ViewType)
	{
		memset(this, 0, sizeof(*this));
		Common.ViewType = ViewType;
	}
};

struct FRHIViewDesc::FBufferSRV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	friend class FRHICommandListBase;
	friend struct FShaderResourceViewInitializer;
	friend struct FRawBufferShaderResourceViewInitializer;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::BufferSRV)
	{}

public:
	FInitializer& SetType(EBufferType Type)
	{
		Buffer.SRV.BufferType = Type;
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Buffer.SRV.Format = InFormat;
		return *this;
	}

	FInitializer& SetOffsetInBytes(uint32 InOffsetBytes)
	{
		Buffer.SRV.OffsetInBytes = InOffsetBytes;
		return *this;
	}

	FInitializer& SetStride(uint32 InStride)
	{
		Buffer.SRV.Stride = InStride;
		return *this;
	}

	FInitializer& SetNumElements(uint32 InNumElements)
	{
		Buffer.SRV.NumElements = InNumElements;
		return *this;
	}
};

struct FRHIViewDesc::FBufferUAV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	friend class FRHICommandListBase;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::BufferUAV)
	{}

public:
	FInitializer& SetType(EBufferType Type)
	{
		Buffer.UAV.BufferType = Type;
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Buffer.UAV.Format = InFormat;
		return *this;
	}

	FInitializer& SetOffsetInBytes(uint32 InOffsetBytes)
	{
		Buffer.UAV.OffsetInBytes = InOffsetBytes;
		return *this;
	}

	FInitializer& SetStride(uint32 InStride)
	{
		Buffer.UAV.Stride = InStride;
		return *this;
	}

	FInitializer& SetNumElements(uint32 InNumElements)
	{
		Buffer.UAV.NumElements = InNumElements;
		return *this;
	}
};

struct FRHIViewDesc::FTextureSRV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	friend class FRHICommandListBase;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::TextureSRV)
	{}

public:
	FInitializer& SetDimension(ETextureDimension InDimension)
	{
		switch (InDimension)
		{
            case ETextureDimension::Texture2DArray  : Texture.SRV.Dimension = EDimension::Texture2DArray  ; break;
            case ETextureDimension::Texture2D       : Texture.SRV.Dimension = EDimension::Texture2D       ; break;
            case ETextureDimension::TextureCube     : Texture.SRV.Dimension = EDimension::TextureCube     ; break;
            case ETextureDimension::TextureCubeArray: Texture.SRV.Dimension = EDimension::TextureCubeArray; break;
            case ETextureDimension::Texture3D       : Texture.SRV.Dimension = EDimension::Texture3D       ; break;
            default: assert(false); break;
        }
		return *this;
	}

	FInitializer& SetDimensionFromTexture(EDimension InDimension)
    {
        Texture.SRV.Dimension = InDimension;
        return *this;
    }

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Texture.SRV.Format = InFormat;
		return *this;
	}

	FInitializer& SetMipRange(uint8 InFirstMip, uint8 InNumMips)
	{
		Texture.SRV.MipRange.First = InFirstMip;
		Texture.SRV.MipRange.Num = InNumMips;
		return *this;
	}

	FInitializer& SetArrayRange(uint16 InFirstElement, uint16 InNumElements)
	{
		Texture.SRV.ArrayRange.First = InFirstElement;
		Texture.SRV.ArrayRange.Num = InNumElements;
		return *this;
	}

	FInitializer& SetDisableSRGB(bool InDisableSRGB)
	{
		Texture.SRV.bDisableSRGB = InDisableSRGB;
		return *this;
	}
};

struct FRHIViewDesc::FTextureUAV::FInitializer : private FRHIViewDesc
{
	friend FRHIViewDesc;
	friend class FRHICommandListBase;

protected:
	FInitializer()
		: FRHIViewDesc(EViewType::TextureUAV)
	{
		// Texture UAVs only support 1 mip
		Texture.UAV.MipRange.Num = 1;
	}

public:
	FInitializer& SetDimension(ETextureDimension InDimension)
	{
		switch (InDimension)
		{
            case ETextureDimension::Texture2D       : Texture.UAV.Dimension = EDimension::Texture2D       ; break;
            case ETextureDimension::Texture2DArray  : Texture.UAV.Dimension = EDimension::Texture2DArray  ; break;
            case ETextureDimension::Texture3D       : Texture.UAV.Dimension = EDimension::Texture3D       ; break;
            case ETextureDimension::TextureCube     : Texture.UAV.Dimension = EDimension::TextureCube     ; break;
            case ETextureDimension::TextureCubeArray: Texture.UAV.Dimension = EDimension::TextureCubeArray; break;
            default: assert(false); break;
		}
		return *this;
	}

	FInitializer& SetFormat(EPixelFormat InFormat)
	{
		Texture.UAV.Format = InFormat;
		return *this;
	}

    /** UAV has only one mip level */
	FInitializer& SetMipLevel(uint8 InMipLevel)
	{
		Texture.UAV.MipRange.First = InMipLevel;
		return *this;
	}

	FInitializer& SetArrayRange(uint16 InFirstElement, uint16 InNumElements)
	{
		Texture.UAV.ArrayRange.First = InFirstElement;
		Texture.UAV.ArrayRange.Num = InNumElements;
		return *this;
	}
};

inline FRHIViewDesc::FBufferSRV::FInitializer FRHIViewDesc::CreateBufferSRV()
{
	return FRHIViewDesc::FBufferSRV::FInitializer();
}

inline FRHIViewDesc::FBufferUAV::FInitializer FRHIViewDesc::CreateBufferUAV()
{
	return FRHIViewDesc::FBufferUAV::FInitializer();
}

inline FRHIViewDesc::FTextureSRV::FInitializer FRHIViewDesc::CreateTextureSRV()
{
	return FRHIViewDesc::FTextureSRV::FInitializer();
}

inline FRHIViewDesc::FTextureUAV::FInitializer FRHIViewDesc::CreateTextureUAV()
{
	return FRHIViewDesc::FTextureUAV::FInitializer();
}

struct FRHIViewDesc::FBuffer::FViewInfo
{
	// The offset in bytes from the beginning of the viewed buffer resource.
	uint32 OffsetInBytes;

	// The size in bytes of a single element in the view.
	uint32 StrideInBytes;

	// The number of elements visible in the view.
	uint32 NumElements;

	// The total number of bytes the data visible in the view covers (i.e. stride * numelements).
	uint32 SizeInBytes;

	// Whether this is a typed / structured / raw view etc.
	EBufferType BufferType;

	// The format of the data exposed by this view. PF_Unknown for all buffer types except typed buffer views.
	EPixelFormat Format;

	// When true, the view is referring to a BUF_NullResource, so a null descriptor should be created.
	bool bNullView;
};

// Buffer SRV specific info
struct FRHIViewDesc::FBufferSRV::FViewInfo : public FRHIViewDesc::FBuffer::FViewInfo
{};

// Buffer UAV specific info
struct FRHIViewDesc::FBufferUAV::FViewInfo : public FRHIViewDesc::FBuffer::FViewInfo
{
	// bool bAtomicCounter = false;
	// bool bAppendBuffer = false;
};

struct FRHIViewDesc::FTexture::FViewInfo
{
    /** The range of array "elements" the view covers. */
	FRHIRange16 ArrayRange;

    /** For read/write, not the actual format of the texture */
	EPixelFormat Format;

	// Specifies how to treat the texture resource when creating the view.
	// E.g. it is possible to create a 2DArray view of a 2D or Cube texture.
	EDimension Dimension : uint32(EDimension::NumBits);

	// True when the view covers every mip of the resource.
	uint8 bAllMips : 1;

	// True when the view covers every array slice of the resource.
	// This includes depth slices for 3D textures, and faces of texture cubes.
	uint8 bAllSlices : 1;
};

// Texture SRV specific info
struct FRHIViewDesc::FTextureSRV::FViewInfo : public FRHIViewDesc::FTexture::FViewInfo
{
	// The range of texture mips the view covers.
	FRHIRange8 MipRange;

	// Indicates if this view should use an sRGB variant of the typed format.
	uint8 bSRGB : 1;
};


// Texture UAV specific info
struct FRHIViewDesc::FTextureUAV::FViewInfo : public FRHIViewDesc::FTexture::FViewInfo
{
	// The single mip level covered by this view.
	uint8 MipLevel;
};

class FRHIView : public FRHIResource
{
public:
	FRHIView(ERHIResourceType InResourceType, FRHIViewableResource* InResource, FRHIViewDesc const& InViewDesc)
		: FRHIResource(InResourceType)
		, Resource(InResource)
		, ViewDesc(InViewDesc)
	{
        assert(InResource);
	}

	// virtual FRHIDescriptorHandle GetBindlessHandle() const
	// {
	// 	return FRHIDescriptorHandle();
	// }

	FRHIViewableResource* GetResource() const
	{
		return Resource.get();
	}

	FRHIBuffer* GetBuffer() const
	{
		return static_cast<FRHIBuffer*>(Resource.get());
	}

	FRHITexture* GetTexture() const
	{
		return static_cast<FRHITexture*>(Resource.get());
	}

	bool IsBuffer () const { return ViewDesc.IsBuffer (); }
	bool IsTexture() const { return ViewDesc.IsTexture(); }

	FRHIViewDesc const& GetDesc() const
	{
		return ViewDesc;
	}

private:
	std::shared_ptr<FRHIViewableResource> Resource;

protected:
	FRHIViewDesc const ViewDesc;
};

class FRHIUnorderedAccessView : public FRHIView
{
public:
	explicit FRHIUnorderedAccessView(FRHIViewableResource* InResource, FRHIViewDesc const& InViewDesc)
		: FRHIView(RRT_UnorderedAccessView, InResource, InViewDesc)
	{
		assert(ViewDesc.IsUAV());
	}
};

class FRHIShaderResourceView : public FRHIView
{
public:
	explicit FRHIShaderResourceView(FRHIViewableResource* InResource, FRHIViewDesc const& InViewDesc)
		: FRHIView(RRT_ShaderResourceView, InResource, InViewDesc)
	{
		assert(ViewDesc.IsSRV());
	}
};

/* 
* Generic staging buffer class used by FRHIGPUMemoryReadback
* RHI specific staging buffers derive from this
*/
class FRHIStagingBuffer : public FRHIResource
{
public:
	FRHIStagingBuffer()
		: FRHIResource(RRT_StagingBuffer)
		, bIsLocked(false)
	{}

	virtual ~FRHIStagingBuffer() {}

    /** Gets a pointer to the locked memory */
	virtual void *Lock(uint32 Offset, uint32 NumBytes) = 0;
	virtual void Unlock() = 0;

protected:
	bool bIsLocked;
};

/** Placeholder class */
class FGenericRHIStagingBuffer : public FRHIStagingBuffer
{
public:
	FGenericRHIStagingBuffer() : FRHIStagingBuffer() {}

	virtual ~FGenericRHIStagingBuffer() {}

	virtual void* Lock(uint32 Offset, uint32 NumBytes) override
	{
		bIsLocked = true;
		return nullptr;
	}

	virtual void Unlock() override
	{
		bIsLocked = false;
	}
};

/** Color render target view */
class FRHIRenderTargetView
{
public:
	FRHITexture* Texture = nullptr;

	ERenderTargetLoadAction LoadAction = ERenderTargetLoadAction::ENoAction;
	ERenderTargetStoreAction StoreAction = ERenderTargetStoreAction::ENoAction;

	FRHIRenderTargetView() = default;
	FRHIRenderTargetView(FRHIRenderTargetView&&) = default;
	FRHIRenderTargetView(const FRHIRenderTargetView&) = default;
	FRHIRenderTargetView& operator=(FRHIRenderTargetView&&) = default;
	FRHIRenderTargetView& operator=(const FRHIRenderTargetView&) = default;
    //common case
	explicit FRHIRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction) :
		Texture(InTexture),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{}
};

/** Depth / Stencil render target view */
class FRHIDepthRenderTargetView
{
public:
	FRHITexture* Texture;

	ERenderTargetLoadAction		DepthLoadAction;
	ERenderTargetStoreAction	DepthStoreAction;
	ERenderTargetLoadAction		StencilLoadAction;
	ERenderTargetStoreAction	StencilStoreAction;
    
    /** Usage of the texture */
	FExclusiveDepthStencil		DepthStencilAccess;

	explicit FRHIDepthRenderTargetView() :
		Texture(nullptr),
		DepthLoadAction(ERenderTargetLoadAction::ENoAction),
		DepthStoreAction(ERenderTargetStoreAction::ENoAction),
		StencilLoadAction(ERenderTargetLoadAction::ENoAction),
		StencilStoreAction(ERenderTargetStoreAction::ENoAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthNop_StencilNop)
	{
	}

	//common case
	explicit FRHIDepthRenderTargetView(
        FRHITexture* InTexture, 
        ERenderTargetLoadAction InLoadAction, 
        ERenderTargetStoreAction InStoreAction) 
        :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture,
        ERenderTargetLoadAction InLoadAction,
        ERenderTargetStoreAction InStoreAction,
        FExclusiveDepthStencil InDepthStencilAccess)
        :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture,
        ERenderTargetLoadAction InDepthLoadAction,
        ERenderTargetStoreAction InDepthStoreAction,
        ERenderTargetLoadAction InStencilLoadAction,
        ERenderTargetStoreAction InStencilStoreAction)
        :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture,
        ERenderTargetLoadAction InDepthLoadAction,
        ERenderTargetStoreAction InDepthStoreAction,
        ERenderTargetLoadAction InStencilLoadAction,
        ERenderTargetStoreAction InStencilStoreAction,
        FExclusiveDepthStencil InDepthStencilAccess)
        :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
	}
};

class FRHISetRenderTargetsInfo
{
public:
	// Color Render Targets Info
	FRHIRenderTargetView ColorRenderTarget[MaxSimultaneousRenderTargets];	
	int32 NumColorRenderTargets;
	bool bClearColor;

	// Color Render Targets Info (for multisampling)
	FRHIRenderTargetView ColorResolveRenderTarget[MaxSimultaneousRenderTargets];	
	bool bHasResolveAttachments;

	// Depth/Stencil Render Target Info
	FRHIDepthRenderTargetView DepthStencilRenderTarget;	
	// Used when depth resolve is enabled.
	FRHIDepthRenderTargetView DepthStencilResolveRenderTarget;
	bool bClearDepth = false;
	bool bClearStencil = false;

	FRHITexture* ShadingRateTexture = nullptr;

	uint8 MultiViewCount = 0;

	FRHISetRenderTargetsInfo() :
		NumColorRenderTargets(0),
		bClearColor(false),
		bHasResolveAttachments(false),
		bClearDepth(false),
		ShadingRateTexture(nullptr),
		MultiViewCount(0)
	{}

	FRHISetRenderTargetsInfo(int32 InNumColorRenderTargets, const FRHIRenderTargetView* InColorRenderTargets, const FRHIDepthRenderTargetView& InDepthStencilRenderTarget) :
		NumColorRenderTargets(InNumColorRenderTargets),
		bClearColor(InNumColorRenderTargets > 0 && InColorRenderTargets[0].LoadAction == ERenderTargetLoadAction::EClear),
		bHasResolveAttachments(false),
		DepthStencilRenderTarget(InDepthStencilRenderTarget),		
		bClearDepth(InDepthStencilRenderTarget.Texture && InDepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::EClear),
		ShadingRateTexture(nullptr),
		MultiViewCount(0)
	{
		for (int32 Index = 0; Index < InNumColorRenderTargets; ++Index)
		{
			ColorRenderTarget[Index] = InColorRenderTargets[Index];			
		}
	}
	// @todo metal mrt: This can go away after all the cleanup is done
	void SetClearDepthStencil(bool bInClearDepth, bool bInClearStencil = false)
	{
		if (bInClearDepth)
		{
			DepthStencilRenderTarget.DepthLoadAction = ERenderTargetLoadAction::EClear;
		}
		if (bInClearStencil)
		{
			DepthStencilRenderTarget.StencilLoadAction = ERenderTargetLoadAction::EClear;
		}
		bClearDepth = bInClearDepth;		
		bClearStencil = bInClearStencil;		
	}
};

/** Shaders in the pipeline state */
struct FBoundShaderStateInput
{
    FRHIVertexDeclaration* VertexDeclarationRHI = nullptr;
    FRHIVertexShader* VertexShaderRHI = nullptr;
    FRHIPixelShader* PixelShaderRHI = nullptr;
    FRHIGeometryShader* GeometryShaderRHI = nullptr;
    FRHIComputeShader* ComputeShaderRHI = nullptr;
};


/**
 * Pipeline States
 * We currently don't have PSO(pipeline state object). We only use its derived class.
 */
class FGraphicsPipelineStateInitializer
{
public:
    FBoundShaderStateInput			BoundShaderState;
    FRHIBlendState*					BlendState;
    FRHIRasterizerState*			RasterizerState;
	FRHIDepthStencilState*			DepthStencilState;
	TArray<FRHISamplerState*>		ImmutableSamplerState; // will not be modified

	EPrimitiveType					PrimitiveType;
	uint32							RenderTargetsEnabled;
    EPixelFormat                    RenderTargetFormats[MaxSimultaneousRenderTargets]; // we have multiple render targets
	ETextureCreateFlags				RenderTargetFlags[MaxSimultaneousRenderTargets];
	EPixelFormat					DepthStencilTargetFormat;
	ETextureCreateFlags				DepthStencilTargetFlag;
	ERenderTargetLoadAction			DepthTargetLoadAction;
	ERenderTargetStoreAction		DepthTargetStoreAction;
	EDepthStencilLoadAction			StencilTargetLoadAction;
	EDepthStencilStoreAction		StencilTargetStoreAction;
	FExclusiveDepthStencil			DepthStencilAccess;
	uint16							NumSamples;
	uint8							MultiViewCount;
};

class FRHIGraphicsPipelineState : public FRHIResource
{
public:
    FRHIGraphicsPipelineState() : FRHIResource(RRT_PipelineState) {}

    virtual FRHIGraphicsShader* GetShader(EShaderType InShaderType) const {}
};

class FRHIGraphicsPipelineStateFallBack : public FRHIGraphicsPipelineState
{
public:
    FRHIGraphicsPipelineStateFallBack(const FGraphicsPipelineStateInitializer& InInitializer)
        : Initializer(InInitializer)
    {}

    virtual FRHIGraphicsShader* GetShader(EShaderType InShaderType) const override
    {
        switch (InShaderType)
        {
        case EShaderType::Vertex:
            return Initializer.BoundShaderState.VertexShaderRHI;
        case EShaderType::Pixel:
            return Initializer.BoundShaderState.PixelShaderRHI;
        case EShaderType::Geometry:
            return Initializer.BoundShaderState.GeometryShaderRHI;
        default:
            return nullptr;
        }
    }

    FGraphicsPipelineStateInitializer Initializer;
};

struct FRHIRenderPassInfo
{
    struct FColorEntry
    {
        FRHITexture* RenderTarget = nullptr;
        FRHITexture* ResolveTarget = nullptr;
        int32 ArraySlice = 0;
        uint8 MipIndex = 0;
        ERenderTargetLoadAction LoadAction = ERenderTargetLoadAction::ENoAction;
        ERenderTargetStoreAction StoreAction = ERenderTargetStoreAction::ENoAction;
    };
	TArray<FColorEntry, TInlineAllocator<FColorEntry, MaxSimultaneousRenderTargets>> ColorRenderTargets;

    struct FDepthStencilEntry
    {
        FRHITexture* DepthStencilTarget = nullptr;
        FRHITexture* ResolveTarget = nullptr;
        EDepthStencilLoadAction LoadAction = EDepthStencilLoadAction::ENoAction;
        EDepthStencilStoreAction StoreAction = EDepthStencilStoreAction::ENoAction;
    };
    FDepthStencilEntry DepthStencilRenderTarget;

    uint8 MultiViewCount = 0;

    /** One color, one depth*/
    explicit FRHIRenderPassInfo(
        FRHITexture* ColorRT, ERenderTargetLoadAction ColorLoadAction, ERenderTargetStoreAction ColorStoreAction,
        FRHITexture* DepthStencilRT = nullptr, EDepthStencilLoadAction DepthLoadAction, EDepthStencilStoreAction DepthStoreAction)
    {
        ColorRenderTargets[0].RenderTarget = ColorRT;
        ColorRenderTargets[0].LoadAction = ColorLoadAction;
        ColorRenderTargets[0].StoreAction = ColorStoreAction;

        DepthStencilRenderTarget.DepthStencilTarget = DepthStencilRT;
        DepthStencilRenderTarget.LoadAction = DepthLoadAction;
        DepthStencilRenderTarget.StoreAction = DepthStoreAction;
    }

    /** One color, no depth */
    explicit FRHIRenderPassInfo(
        FRHITexture* ColorRT, ERenderTargetLoadAction ColorLoadAction, ERenderTargetStoreAction ColorStoreAction)
    {
        ColorRenderTargets[0].RenderTarget = ColorRT;
        ColorRenderTargets[0].LoadAction = ColorLoadAction;
        ColorRenderTargets[0].StoreAction = ColorStoreAction;
    }

    /** No color, one depth */
    explicit FRHIRenderPassInfo(
        FRHITexture* DepthStencilRT, EDepthStencilLoadAction DepthLoadAction, EDepthStencilStoreAction DepthStoreAction)
    {
        DepthStencilRenderTarget.DepthStencilTarget = DepthStencilRT;
        DepthStencilRenderTarget.LoadAction = DepthLoadAction;
        DepthStencilRenderTarget.StoreAction = DepthStoreAction;
    }

    /** Multiple colors, one depth */
    explicit FRHIRenderPassInfo(
        int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetLoadAction ColorLoadAction, ERenderTargetStoreAction ColorStoreAction,
        FRHITexture* DepthStencilRT, EDepthStencilLoadAction DepthLoadAction, EDepthStencilStoreAction DepthStoreAction)
    {
        assert(0 <= NumColorRTs && NumColorRTs <= MaxSimultaneousRenderTargets);
        for (int32 i = 0; i < NumColorRTs; ++i)
        {
            ColorRenderTargets[i].RenderTarget = ColorRTs[i];
            ColorRenderTargets[i].LoadAction = ColorLoadAction;
            ColorRenderTargets[i].StoreAction = ColorStoreAction;
        }

        DepthStencilRenderTarget.DepthStencilTarget = DepthStencilRT;
        DepthStencilRenderTarget.LoadAction = DepthLoadAction;
        DepthStencilRenderTarget.StoreAction = DepthStoreAction;
    }

    /** Multiple colors, no depth */
    explicit FRHIRenderPassInfo(
        int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetLoadAction ColorLoadAction, ERenderTargetStoreAction ColorStoreAction
    )
    {
        assert(0 <= NumColorRTs && NumColorRTs <= MaxSimultaneousRenderTargets);
        for (int32 i = 0; i < NumColorRTs; ++i)
        {
            ColorRenderTargets[i].RenderTarget = ColorRTs[i];
            ColorRenderTargets[i].LoadAction = ColorLoadAction;
            ColorRenderTargets[i].StoreAction = ColorStoreAction;
        }
    }

	FRHIRenderPassInfo() = default;
	FRHIRenderPassInfo(FRHIRenderPassInfo&&) = default;
	FRHIRenderPassInfo(const FRHIRenderPassInfo&) = default;
	FRHIRenderPassInfo& operator=(FRHIRenderPassInfo&&) = default;
	FRHIRenderPassInfo& operator=(const FRHIRenderPassInfo&) = default;
};

/** Descriptor used to create a buffer resource */
using FRHIBufferCreateInfo = FRHIBufferDesc;

struct FRHITextureSRVCreateInfo
{
    EPixelFormat Format;
    uint8 MipLevel;
    uint8 NumMipLevels;
    uint8 FirstArraySlice;
    uint8 NumArraySlices;
    ETextureDimension Dimension;
};

struct FRHITextureUAVCreateInfo
{
    EPixelFormat Format;
    uint8 MipLevel;
    uint16 FirstArraySlice;
    uint16 NumArraySlices;
};
