#pragma once

#include "HAL/PlatformType.h"
#include "Container/Array.h"
#include "CoreUObject/UObject/NameTypes.h"
#include "PixelFormat.h"
#include "RHIDefinitions.h"

namespace RHI
{
// 필요한가??
struct IRHIBindable
{
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    bool IsBound() const { return bIsBound; }

protected:
    bool bIsBound = false;
    uint32 BindSlot = 0; // slot to which the resource is bound
};

struct FRHIView
{
    EViewTarget ViewTarget;
    EViewType ViewType;
    EPixelFormat Format = EPixelFormat::PF_Unknown;

    /** These fields are ignored in RTVs */
    uint8 MostDetailedMip = 0;
    uint8 MipLevels = 1;

    virtual void Release() = 0;
};


struct FRHIResource
{
public:
    virtual ~FRHIResource() = default;
    virtual void Release() = 0;
};

// -----------------------------------------------------------------------------
// Buffer
// -----------------------------------------------------------------------------
struct FRHIBufferDesc
{
    uint32 Size = 0;
    EBufferUsage Usage = EBufferUsage::Default;
    ECPUAccess CPUAccess = ECPUAccess::Read;
    EBindFlag BindFlag = EBindFlag::None;
};

struct FRHIBuffer : public FRHIResource, public IRHIBindable
{
public:
    FRHIBuffer(FRHIBufferDesc InDesc) {}

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

    FRHIBufferDesc GetDesc() const { return Desc; }

protected:
    FRHIBufferDesc Desc;
};

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------
struct FAttribute
{
    EAttributeType Type; // data type
    EAttributeUsage Usage; // semantic usage
    uint32 Index; // for usages that can have multiple (e.g., TexCoord0, TexCoord1)
    uint32 Offset; // offset in bytes from the start of the vertex
    FAttribute(EAttributeType InType, EAttributeUsage InUsage, uint32 InIndex, uint32 InOffset)
        : Type(InType), Usage(InUsage), Index(InIndex), Offset(InOffset)
    {
    }
};

struct FRHIInputLayout : public FRHIResource
{
    TArray<FAttribute> Attributes;
    uint32 Stride = 0; // stride between elements
};

// -----------------------------------------------------------------------------
// Shader
// -----------------------------------------------------------------------------
struct FRHIShaderDesc
{
    FString EntryPoint;
    TArray<FString> Defines;
    FString FilePath;
};

struct FRHIShader : public FRHIResource
{
    FRHIShader(FRHIShaderDesc InDesc) : Desc(InDesc) {}

    FRHIShaderDesc Desc;
};

struct FRHIVertexShader : public FRHIShader
{
    FRHIVertexShader(FRHIShaderDesc InDesc) : FRHIShader(InDesc) {}
};

struct FRHIPixelShader : public FRHIShader
{
    FRHIPixelShader(FRHIShaderDesc InDesc) : FRHIShader(InDesc) {}
};

struct FRHIComputeShader : public FRHIShader
{
    FRHIComputeShader(FRHIShaderDesc InDesc) : FRHIShader(InDesc) {}
};

struct FRHIGeometryShader : public FRHIShader
{
    FRHIGeometryShader(FRHIShaderDesc InDesc) : FRHIShader(InDesc) {}
};

// -----------------------------------------------------------------------------
// Texture / Sampler
// -----------------------------------------------------------------------------
struct FRHITextureDesc
{
    ETextureType Type = ETextureType::Texture2D;
    EPixelFormat Format = EPixelFormat::PF_Unknown;
    uint32 Width = 0;
    uint32 Height = 0;
    uint32 Depth = 1; // For 3D textures
    uint32 MostDetailedMip = 0;
    uint32 MipLevels = 1;
    uint32 ArraySize = 1; // For texture arrays or cube maps

    FRHITextureDesc(ETextureType InType, EPixelFormat InFormat, uint32 InWidth, uint32 InHeight, uint32 InDepth = 1, uint32 InMipLevels = 1, uint32 InArraySize = 1)
        : Type(InType), Format(InFormat), Width(InWidth), Height(InHeight), Depth(InDepth), MipLevels(InMipLevels), ArraySize(InArraySize)
    {
    }
};

struct FRHITexture : public FRHIResource
{
    FRHITextureDesc Desc;

    FRHITexture(const FRHITextureDesc& InDesc) : Desc(InDesc) {}

    FRHIView* SRV = nullptr;
    FRHIView* RTV = nullptr;
    FRHIView* DSV = nullptr;
    FRHIView* UAV = nullptr;
};

struct FRHISamplerStateDesc
{
    ESamplerFilter Filter = ESamplerFilter::Linear;
    ESamplerAddressMode AddressU = ESamplerAddressMode::Wrap;
    ESamplerAddressMode AddressV = ESamplerAddressMode::Wrap;
    ESamplerAddressMode AddressW = ESamplerAddressMode::Wrap;

    float MipLODBias = 0.0f;
    uint32 MaxAnisotropy = 1;
    float MinLOD = 0.0f;
    float MaxLOD = FLT_MAX;
    uint32 BorderColor = 0xFFFFFFFF; // RGBA
    bool bUseComparison = false;
};
struct FRHISamplerState : public FRHIResource, public IRHIBindable
{
    FRHISamplerStateDesc Desc;

    FRHISamplerState(const FRHISamplerStateDesc& InDesc) : Desc(InDesc) {}
};

// -----------------------------------------------------------------------------
// States 
// -----------------------------------------------------------------------------
struct FRHIRasterizerStateDesc
{
    EFillMode FillMode = EFillMode::Solid;
    ECullMode CullMode = ECullMode::Back;
    bool bFrontCounterClockwise = false;
    int32 DepthBias = 0;
    float DepthBiasClamp = 0.0f;
    float SlopeScaledDepthBias = 0.0f;
};

struct FRHIRasterizerState : public FRHIResource
{
    FRHIRasterizerStateDesc Desc;
    FRHIRasterizerState(FRHIRasterizerStateDesc InDesc) : Desc(InDesc) {}
};

struct FRHIBlendStateDesc
{
    bool bAlphaToCoverage = false;
    bool bIndependentBlend = false;
    bool bEnableBlend = false;
    EBlendFactor SrcBlend = EBlendFactor::One;
    EBlendFactor DestBlend = EBlendFactor::Zero;
    EBlendOp BlendOp = EBlendOp::Add;
    EBlendFactor SrcBlendAlpha = EBlendFactor::One;
    EBlendFactor DestBlendAlpha = EBlendFactor::Zero;
    EBlendOp BlendOpAlpha = EBlendOp::Add;
    uint8 RenderTargetWriteMask = 0b1111; // RGBA
};

struct FRHIBlendState : public FRHIResource
{
    FRHIBlendStateDesc Desc;
    FRHIBlendState(FRHIBlendStateDesc InDesc) : Desc(InDesc) {}
};

struct FRHIDepthStencilStateDesc
{
    /*
    * Depth test settings
    */
    bool bEnableDepth = true;
    bool bDepthWriteMask = true;
    EComparisonFunc DepthFunc = EComparisonFunc::Less;

    /*
    * Stencil test settings
    */
    bool bEnableStencil = false;
    uint8 StencilReadMask = 0xFF;
    uint8 StencilWriteMask = 0xFF;

    // Front face
    EComparisonFunc FrontFaceStencilFunc = EComparisonFunc::Always;
    EStencilOp FrontFaceStnecilFailOp = EStencilOp::Keep;
    EStencilOp FrontFaceStencilDepthFailOp = EStencilOp::Incr;
    EStencilOp FrontFaceStencilPassOp = EStencilOp::Keep;

    // Back face
    EComparisonFunc BackFaceStencilFunc = EComparisonFunc::Always;
    EStencilOp BackFaceStencilFailOp = EStencilOp::Keep;
    EStencilOp BackFaceStencilDepthFailOp = EStencilOp::Decr;
    EStencilOp BackFaceStencilPassOp = EStencilOp::Keep;
};

struct FRHIDepthStencilState : public FRHIResource
{
    FRHIDepthStencilStateDesc Desc;
    FRHIDepthStencilState(FRHIDepthStencilStateDesc InDesc) : Desc(InDesc) {}
};

// -----------------------------------------------------------------------------
// Uniform / Constant buffer
// -----------------------------------------------------------------------------
struct FUniformBufferPlainMember
{
    EUniformBufferBaseType Type;
    uint16 Offset;      // Byte offset in constant buffer
    uint16 Size;        // Padded size for this member
    FName  Name;
};

struct FUniformBufferResourceMember
{
    EUniformBufferBaseType Type;    // Texture / Sampler
    uint16 Index;                   // Resource binding array index
    FName  Name;
};

struct FRHIUniformBufferLayout
{
    uint32 ConstantBufferSize = 0;                          // Padded Size
    TArray<FUniformBufferPlainMember>    PlainMembers;      // Primitives (float, int, mat, struct)
    TArray<FUniformBufferResourceMember> ResourceMembers;   // Texture / Sampler
};

class FRHIUniformBuffer : public FRHIResource
{
public:
    FRHIUniformBuffer(const FRHIBufferDesc& InDesc,
        const FRHIUniformBufferLayout* InLayout,
        EUniformBufferUsage InUsage)
        : Desc(InDesc), Layout(InLayout), Usage(InUsage) {
    }

    virtual ~FRHIUniformBuffer() = default;

    const FRHIUniformBufferLayout& GetLayout() const { return *Layout; }
    EUniformBufferUsage GetUsage() const { return Usage; }
    const FRHIBufferDesc& GetBufferDesc() const { return Desc; }

    // Update entire constant data (Dynamic / SingleFrame only)
    virtual void UpdateContents(const void* Data, uint32 Size) = 0;

protected:
    FRHIBufferDesc Desc;
    const FRHIUniformBufferLayout* Layout;
    EUniformBufferUsage Usage;
};

// -----------------------------------------------------------------------------
// Viewport (SwapChain)
// -----------------------------------------------------------------------------
struct FRHIViewportDesc
{
    uint32 Width = 0;
    uint32 Height = 0;
    EPixelFormat Format = EPixelFormat::PF_Unknown;
    bool bEnableVSync = true;
};

struct FRHIViewport : public FRHIResource
{
    FRHIViewportDesc Desc;
    FRHIViewport(FRHIViewportDesc InDesc) : Desc(InDesc) {}

    virtual void* GetNativeBackBufferTexture() const = 0;
    virtual void* GetNativeSwapChain() const = 0;
    virtual void* GetNativeWindow() const = 0;

    virtual void Tick(float DeltaTime) = 0;
};
} // namespace RHI
