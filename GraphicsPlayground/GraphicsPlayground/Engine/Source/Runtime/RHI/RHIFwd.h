#pragma once

#include <memory>
#include "Core/Templates/RefCounting.h"

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------
// class FRHICommandListBase;
// class FRHICommandList;
// class FRHICommandListImmediate;
// class IRHICommandContext;

// -------------------------------------------------------------------------
// Base / Core
// -------------------------------------------------------------------------
struct FRHIResource;

// Clear helpers
enum class EClearBinding;
struct FClearValueBinding;

// Depth/Stencil mode wrapper
class FExclusiveDepthStencil;

// -------------------------------------------------------------------------
// State Objects
// -------------------------------------------------------------------------
class FRHISamplerState;
class FRHIRasterizerState;
class FRHIDepthStencilState;
class FRHIBlendState;

// -------------------------------------------------------------------------
// Shader Bindings / Shaders
// -------------------------------------------------------------------------
struct FRHIVertexDeclaration;
struct FRHIBoundShaderState;

struct FShaderResourceTable;
struct FRHIShaderData;
struct FRHIShader;
class  FRHIGraphicsShader;
struct FRHIVertexShader;
struct FRHIPixelShader;
struct FRHIGeometryShader;
struct FRHIComputeShader;

// -------------------------------------------------------------------------
// Uniform Buffers
// -------------------------------------------------------------------------
struct FRHIUniformBufferResource;
struct FRHIUniformBufferResourceInitializer;
struct FRHIUniformBufferLayoutInitializer;
struct FRHIUniformBufferLayout;
struct FRHIUniformBuffer;

// -------------------------------------------------------------------------
// Buffers
// -------------------------------------------------------------------------
struct FRHIViewableResource;
struct FRHIBufferDesc;
struct FRHIBufferCreateDesc;
struct FRHIBuffer;
class  FRHIStreamSourceSlot;

// -------------------------------------------------------------------------
// Textures
// -------------------------------------------------------------------------
struct FRHITextureDesc;
struct FRHITextureCreateDesc;
class  FRHITexture;

// -------------------------------------------------------------------------
// Viewport / SwapChain
// -------------------------------------------------------------------------
class FRHIViewport;

// -------------------------------------------------------------------------
// Views (SRV / UAV abstraction)
// -------------------------------------------------------------------------
struct FRHIRange16;
struct FRHIRange8;
struct FRHIViewDesc;

class FRHIView; // Base view
class FRHIUnorderedAccessView;
class FRHIShaderResourceView;

// Staging buffer
class FRHIStagingBuffer;

// -------------------------------------------------------------------------
// Render Targets & Render Pass Setup
// -------------------------------------------------------------------------
class FRHIRenderTargetView;
class FRHIDepthRenderTargetView;
class FRHISetRenderTargetsInfo;

// -------------------------------------------------------------------------
// Pipeline State (legacy style & simplified PSO substitute)
// -------------------------------------------------------------------------
struct FBoundShaderStateInput;
class  FGraphicsPipelineStateInitializer;
class  FRHIGraphicsPipelineState;
class  FRHIGraphicsPipelineStateFallBack;

// -------------------------------------------------------------------------
// Render Pass Info
// -------------------------------------------------------------------------
struct FRHIRenderPassInfo;

// -------------------------------------------------------------------------
// Aliases / Helper Descriptors
// -------------------------------------------------------------------------
using FRHIBufferCreateInfo = FRHIBufferDesc;

struct FRHITextureSRVCreateInfo;
struct FRHITextureUAVCreateInfo;

using FResourceRHIRef                = TRefCountPtr<FRHIResource>;
using FSamplerStateRHIRef            = TRefCountPtr<FRHISamplerState>;
using FRasterizerStateRHIRef         = TRefCountPtr<FRHIRasterizerState>;
using FDepthStencilStateRHIRef       = TRefCountPtr<FRHIDepthStencilState>;
using FBlendStateRHIRef              = TRefCountPtr<FRHIBlendState>;
using FVertexDeclarationRHIRef       = TRefCountPtr<FRHIVertexDeclaration>;
using FBoundShaderStateRHIRef        = TRefCountPtr<FRHIBoundShaderState>;
using FShaderRHIRef                  = TRefCountPtr<FRHIShader>;
using FGraphicsShaderRHIRef          = TRefCountPtr<FRHIGraphicsShader>;
using FVertexShaderRHIRef            = TRefCountPtr<FRHIVertexShader>;
using FPixelShaderRHIRef             = TRefCountPtr<FRHIPixelShader>;
using FGeometryShaderRHIRef          = TRefCountPtr<FRHIGeometryShader>;
using FComputeShaderRHIRef           = TRefCountPtr<FRHIComputeShader>;
using FUniformBufferLayoutRHIRef     = TRefCountPtr<FRHIUniformBufferLayout>;
using FUniformBufferRHIRef           = TRefCountPtr<FRHIUniformBuffer>;
using FBufferRHIRef                  = TRefCountPtr<FRHIBuffer>;
using FStreamSourceSlotRHIRef        = TRefCountPtr<FRHIStreamSourceSlot>;
using FTextureRHIRef                 = TRefCountPtr<FRHITexture>;
using FViewportRHIRef                = TRefCountPtr<FRHIViewport>;
using FViewRHIRef                    = TRefCountPtr<FRHIView>;
using FUnorderedAccessViewRHIRef     = TRefCountPtr<FRHIUnorderedAccessView>;
using FShaderResourceViewRHIRef      = TRefCountPtr<FRHIShaderResourceView>;
using FStagingBufferRHIRef           = TRefCountPtr<FRHIStagingBuffer>;
using FRenderTargetViewRHIRef        = TRefCountPtr<FRHIRenderTargetView>;
using FDepthRenderTargetViewRHIRef   = TRefCountPtr<FRHIDepthRenderTargetView>;
using FSetRenderTargetsInfoRHIRef    = TRefCountPtr<FRHISetRenderTargetsInfo>;
using FGraphicsPipelineStateRHIRef   = TRefCountPtr<FRHIGraphicsPipelineState>;
using FGraphicsPipelineStateFallBackRHIRef = TRefCountPtr<FRHIGraphicsPipelineStateFallBack>;
using FRenderPassInfoRHIRef          = TRefCountPtr<FRHIRenderPassInfo>;
