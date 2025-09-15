#pragma once

#include "HAL/PlatformType.h"
#include "Core/Misc/EnumClassFlags.h"
#include "RHI/RHIFwd.h"
#include "Math/Color.h"

// This value has to be changed if we use deferred rendering
#define MAX_SIMULTANEOUS_RENDER_TARGETS 2

/**
 * 최대한 API-independent하게 flag로 작성하고 RHI의 변수에 flag를 저장하기보단,
 * API별 initializer에서 직접 확인해서 해당하는 API를 호출
 */

/** An enumeration of the different RHI reference types. */
enum ERHIResourceType : uint8
{
	RRT_None,

	RRT_SamplerState,
	RRT_RasterizerState,
	RRT_DepthStencilState,
	RRT_BlendState,
	RRT_VertexDeclaration,
	RRT_VertexShader,
	RRT_PixelShader,
	RRT_GeometryShader,
	RRT_ComputeShader,
	RRT_GraphicsPipelineState,
	RRT_ComputePipelineState,
	RRT_RayTracingPipelineState,
	RRT_BoundShaderState,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_Buffer,
    RRT_StreamSourceSlot,
	RRT_Texture,
	RRT_Texture2D,
	RRT_Texture2DArray,
	RRT_Texture3D,
	RRT_TextureCube,
	RRT_Viewport,
	RRT_UnorderedAccessView,
	RRT_ShaderResourceView,
	RRT_StagingBuffer,
    RRT_PipelineState,

	RRT_Num
};

enum class ETextureDimension : uint8
{
	Texture2D,
	Texture2DArray,
	Texture3D,
	TextureCube,
	TextureCubeArray
};

/**
 * Flag used to give specific buffer usage hints.
 * Flags not compatible with ERHIAccess will make assert.
 */
enum class EBufferUsageFlags : uint16
{
	None                    = 0,
	/** The buffer will be written to once. */
	Static                  = 1 << 0,

	/** The buffer will be written to occasionally, GPU read only, CPU write only.  The data lifetime is until the next update, or the buffer is destroyed. */
	Dynamic                 = 1 << 1,

	/** The buffer's data will have a lifetime of one frame.  It MUST be written to each frame, or a new one created each frame. */
	Volatile                = 1 << 2,

	/** Allows an unordered access view to be created for the buffer. */
	UnorderedAccess         = 1 << 3,

	/** 
	 * Create a buffer that can be bound as a shader resource. 
	 * This is only needed for buffer types which wouldn't ordinarily be used as a shader resource, like a vertex buffer.
	 */	
    ShaderResource          = 1 << 4,

	/** Request that this buffer is directly CPU accessible. */
	KeepCPUAccessible       = 1 << 5,

	VertexBuffer            = 1 << 6,
	IndexBuffer             = 1 << 7,
	StructuredBuffer        = 1 << 8,

    /** Buffer can be used as uniform buffer on platforms that do support uniform buffer objects. */
    UniformBuffer           = 1 << 9,

	/** The buffer is a placeholder for streaming, and does not contain an underlying GPU resource. */
	NullResource            = 1 << 10,

	// Helper bit-masks
	AnyDynamic              = (Dynamic | Volatile),
};
ENUM_CLASS_FLAGS(EBufferUsageFlags)

enum class ETextureCreateFlags : uint8
{
    // ready-only static texture
    None                              = 0,
    // for RTV
    RenderTargetable                  = 1u << 0,
    // for DSV
    DepthStencilTargetable            = 1u << 1,
    // for SRV
    ShaderResource                    = 1u << 2,
    // sRGB
    SRGB                              = 1u << 3,
    // writable after buffer create.
    CPUWritable                       = 1u << 4,
	// expected to update(write) every frame
    Dynamic                           = 1u << 5,
    // DXD11 only
    UAV                               = 1u << 6,
    // Texture data is accessible by the CPU.
    CPUReadback                       = 1u << 7,
};
ENUM_CLASS_FLAGS(ETextureCreateFlags);

enum
{
    MaxSimultaneousRenderTargets = MAX_SIMULTANEOUS_RENDER_TARGETS,
    MaxSimultaneousRenderTargets_NumBits = 3,
};

/**
 * Action to take when a render target is set.
 */
enum class ERenderTargetLoadAction : uint8
{
	// Untouched contents of the render target are undefined. Any existing content is not preserved.
	ENoAction,

	// Existing contents are preserved.
	ELoad,

	// The render target is cleared to the fast clear value specified on the resource.
	EClear,

	Num,
	NumBits = 2,
};

/**
 * Action to take when a depth stencil is set.
 */
enum class EDepthStencilLoadAction : uint8
{
	// Untouched contents of the depth stencil are undefined. Any existing content is not preserved.
	ENoAction,

	// Existing contents are preserved.
	ELoad,

	// The render target is cleared to the fast clear value specified on the resource.
	EClear,

	Num,
	NumBits = 2,
};

/**
 * Action to take when a render target is unset or at the end of a pass. 
 */
enum class ERenderTargetStoreAction : uint8
{
	// Contents of the render target emitted during the pass are not stored back to memory.
	ENoAction,

	// Contents of the render target emitted during the pass are stored back to memory.
	EStore,

	// Contents of the render target emitted during the pass are resolved using a box filter and stored back to memory.
	EMultisampleResolve,

	Num,
	NumBits = 2,
};

/**
 * Action to take when a depth stencil is unset or at the end of a pass. 
 */
enum class EDepthStencilStoreAction : uint8
{
	// Contents of the depth stencil emitted during the pass are not stored back to memory.
	ENoAction,

	// Contents of the depth stencil emitted during the pass are stored back to memory.
	EStore,

	// Contents of the depth stencil emitted during the pass are resolved using a box filter and stored back to memory.
	EMultisampleResolve,

	Num,
	NumBits = 2,
};

/**
 * Common render target use cases
 */
enum class ESimpleRenderTargetMode
{
	// These will all store out color and depth
	EExistingColorAndDepth,							// Color = Existing, Depth = Existing
	EUninitializedColorAndDepth,					// Color = ????, Depth = ????
	EUninitializedColorExistingDepth,				// Color = ????, Depth = Existing
	EUninitializedColorClearDepth,					// Color = ????, Depth = Default
	EClearColorExistingDepth,						// Clear Color = whatever was bound to the rendertarget at creation time. Depth = Existing
	EClearColorAndDepth,							// Clear color and depth to bound clear values.
	EExistingContents_NoDepthStore,					// Load existing contents, but don't store depth out.  depth can be written.
	EExistingColorAndClearDepth,					// Color = Existing, Depth = clear value
	EExistingColorAndDepthAndClearStencil,			// Color = Existing, Depth = Existing, Stencil = clear

	// If you add an item here, make sure to add it to DecodeRenderTargetMode() as well!
};

enum class EClearDepthStencil
{
	Depth,
	Stencil,
	DepthStencil,
};

enum EPrimitiveType : uint8
{
    PT_TriangleList,
    PT_TriangleStrip,
    PT_LineList,
    PT_LineStrip,
    PT_PointList
};

using FUniformBufferStaticSlot = uint8;
using FShaderResourceStaticSlot = uint8;
using FSamplerStaticSlot = uint8;

enum class EShaderType : uint8
{
    Vertex,
    Pixel,
    Compute,
    Geometry,

    NumBits = 2
};

// -----------------------------------------------------------------------------
// View
// -----------------------------------------------------------------------------
enum EViewTarget : uint8
{
    SRV,
    RTV,
    DSV,
    UAV
};

enum class EViewType : uint8
{
    Buffer,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
    Texture3DArray,
    TextureCube
};

// -----------------------------------------------------------------------------
// Buffers
// -----------------------------------------------------------------------------

// enum class EBindFlag : uint8
// {
//     None = 0x00,
//     Vertex = 0x01,
//     Index = 0x02,
//     //Uniform = 0x04, // use FRHIUniformBuffer instead.
//     SRV = 0x08,
//     RTV = 0x10,
//     DSV = 0x20,
//     UAV = 0x40,
// };

// -----------------------------------------------------------------------------
// Input Layout
// -----------------------------------------------------------------------------
enum class EVertexElementType : uint8
{
    Float,      // 32-bit float
    Float2,     // 2 x 32-bit float
    Float3,     // 3 x 32-bit float
    Float4,     // 4 x 32-bit float
    Int,        // 32-bit int
    Int2,       // 2 x 32-bit int
    Int3,       // 3 x 32-bit int
    Int4,       // 4 x 32-bit int
    UInt,       // 32-bit unsigned int
    UInt2,      // 2 x 32-bit unsigned int
    UInt3,      // 3 x 32-bit unsigned int
    UInt4,      // 4 x 32-bit unsigned int
    Byte,       // 8-bit byte
    Short,      // 16-bit short
    Long,       // 32-bit long
    LongLong,   // 64-bit long long
};

// only use slot
// enum class EAttributeUsage : uint8
// {
//     Position,
//     Normal,
//     TexCoord,
//     Color,
//     Tangent,
//     Bitangent,
//     BoneWeights,
//     BoneIndices,
//     Custom
// };

// -----------------------------------------------------------------------------
// Rasterizer
// -----------------------------------------------------------------------------
enum class EFillMode : uint8
{
    Wireframe,
    Solid
};

enum class ECullMode : uint8
{
    None,
    Front,
    Back
};

// -----------------------------------------------------------------------------
// Texture / Sampler
// -----------------------------------------------------------------------------
enum class ETextureType : uint8
{
    Buffer,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
    Texture3DArray,
    TextureCube
};

enum class ETextureUsage : uint8
{
    Default = 0x01,
    RenderTarget = 0x02,
    DepthStencil = 0x04,
    ShaderResource = 0x08,
    UnorderedAccess = 0x10
};

enum class ESamplerFilter : uint8
{
    Point,
    Bilinear,
    Trilinear,
    Anisotropic
};

enum class ESamplerAddressMode : uint8
{
    Wrap,
    Clamp,
    Mirror,
    Border
};

// -----------------------------------------------------------------------------
// Blend
// -----------------------------------------------------------------------------
enum class EBlendFactor : uint8
{
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
    BlendFactor,
    InvBlendFactor,
    Src1Color,
    InvSrc1Color,
    Src1Alpha,
    InvSrc1Alpha
};

enum class EBlendOp : uint8
{
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max
};

// -----------------------------------------------------------------------------
// Depth Stencil
// -----------------------------------------------------------------------------
enum class EComparisonFunc : uint8
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

enum class EStencilOp : uint8
{
    Keep,
    Zero,
    Replace,
    IncrSat,
    DecrSat,
    Invert,
    Incr,
    Decr
};

// -----------------------------------------------------------------------------
// Uniform Buffer
// -----------------------------------------------------------------------------
enum class EUniformBufferUsage : uint8
{
    Static,
    Dynamic,
    SingleFrame
};

enum EUniformBufferBaseType : uint8
{
	UBMT_INVALID,

    /** Shader does not take boolean values. Use integer instead. */
	// UBMT_BOOL,

	UBMT_INT32,
	UBMT_UINT32,
	UBMT_FLOAT32,

	UBMT_TEXTURE,
	UBMT_SRV,
	UBMT_UAV,
	UBMT_SAMPLER,
};

enum class EUniformBufferBindingFlags : uint8
{
	/** Bound on specific shader (i.e. RHISetShaderUniformBuffer). */
	Shader = 0,

	/** Globally(Static) set (i.e. RHISetStaticUniformBuffers). */
	Static = 1,

    NumBits = 1
};
