#pragma once

#include "HAL/PlatformType.h"
#include "RHI/RHIFwd.h"
#include "Math/Color.h"

namespace RHI
{
enum class EPrimitiveType : uint8
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
    Geometry
};

struct FRHIRenderPassInfo
{
    uint32 NumColorRenderTargets = 0;
    FRHIView* ColorRenderTargets[8] = { nullptr }; // 최대 8개의 렌더 타겟 지원
    FRHIView* DepthStencilRenderTarget = nullptr;
    // Clear options
    bool bClearColor = false;
    FLinearColor ClearColor = FLinearColor::Black;
    bool bClearDepth = false;
    float ClearDepth = 1.0f;
    bool bClearStencil = false;
    uint8 ClearStencil = 0;
    FRHIRenderPassInfo() = default;
    FRHIRenderPassInfo(uint32 InNumColorRTs, FRHIView* const* InColorRTs, FRHIView* InDSV)
        : NumColorRenderTargets(InNumColorRTs)
        , DepthStencilRenderTarget(InDSV)
    {
        for (uint32 i = 0; i < InNumColorRTs && i < 8; ++i)
        {
            ColorRenderTargets[i] = InColorRTs[i];
        }
    };
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
enum class EBufferUsage : uint8
{
    Default,
    Immutable,
    Dynamic,
    Staging
};

enum class ECPUAccess : uint8
{
    Read,
    Write
};

enum class EBindFlag : uint8
{
    None = 0x00,
    Vertex = 0x01,
    Index = 0x02,
    //Uniform = 0x04, // use FRHIUniformBuffer instead.
    SRV = 0x08,
    RTV = 0x10,
    DSV = 0x20,
    UAV = 0x40,
};

// -----------------------------------------------------------------------------
// Input Layout
// -----------------------------------------------------------------------------
enum class EAttributeType : uint8
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

enum class EAttributeUsage : uint8
{
    Position,
    Normal,
    TexCoord,
    Color,
    Tangent,
    Bitangent,
    BoneWeights,
    BoneIndices,
    Custom
};

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
    Linear,
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

enum class EUniformBufferBaseType : uint8
{
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
    Mat3, Mat4,
    Texture,
    Sampler,
    Struct,
    Unknown
};
} // namespace RHI
