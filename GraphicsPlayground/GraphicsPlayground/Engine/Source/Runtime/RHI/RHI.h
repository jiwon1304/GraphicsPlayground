#pragma once

#include "RHI/RHIDefinitions.h"
#include "CoreUObject/UObject/NameTypes.h"
#include "Core/Container/Array.h"

struct FRHIResourceStats
{
	FName Name;

	uint64	SizeInBytes = 0;
	bool	bRenderTarget = false;
	bool	bDepthStencil = false;
	bool	bUnorderedAccessView = false;

};

/** We do not use semantics: just slots */
struct FVertexElement
{
    uint8 StreamIndex; // Vertex Buffer Slot
    uint8 Offset; // Offset in bytes from the beginning of the vertex
    EVertexElementType Type; // Data type
    uint8 AttributeIndex;
    uint16 Stride;

    FVertexElement() {}
    FVertexElement(uint8 InStreamIndex, uint8 InOffset, EVertexElementType InType, uint8 InAttributeIndex, uint16 InStride)
        : StreamIndex(InStreamIndex)
        , Offset(InOffset)
        , Type(InType)
        , AttributeIndex(InAttributeIndex)
        , Stride(InStride)
    {}
};

using FVertexDeclarationElementList = TArray<FVertexElement>;

struct FSamplerStateInitializerRHI
{
    ESamplerFilter Filter = ESamplerFilter::Bilinear;
    ESamplerAddressMode AddressU = ESamplerAddressMode::Wrap;
    ESamplerAddressMode AddressV = ESamplerAddressMode::Wrap;
    ESamplerAddressMode AddressW = ESamplerAddressMode::Wrap;

    float MipLODBias = 0.0f;
    uint32 MaxAnisotropy = 1;
    float MinLOD = 0.0f;
    float MaxLOD = FLT_MAX;
    uint32 BorderColor = 0x00000000; // white
    bool bUseComparison = false;
};

struct FRasterizerStateInitializerRHI
{
    EFillMode FillMode = EFillMode::Solid;
    ECullMode CullMode = ECullMode::Back;
    bool bFrontCounterClockwise = false;
    int32 DepthBias = 0;
    float DepthBiasClamp = 0.0f;
    float SlopeScaledDepthBias = 0.0f;
};

struct FBlendStateInitializerRHI
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

struct FDepthStencilStateInitializerRHI
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