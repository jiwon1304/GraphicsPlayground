#pragma once

#include <memory>
//#include "RHIResources.h"

namespace RHI
{
struct IRHIBindable;
struct FRHIView;
struct FRHIResource;
struct FRHIBufferDesc;
struct FRHIBuffer;
struct FRHIInputLayout;
struct FRHIShaderDesc;
struct FRHIShader;
struct FRHIVertexShader;
struct FRHIPixelShader;
struct FRHIComputeShader;
struct FRHIGeometryShader;
struct FRHITextureDesc;
struct FRHITexture;
struct FRHISamplerStateDesc;
struct FRHISamplerState;
struct FRHIRasterizerStateDesc;
struct FRHIRasterizerState;
struct FRHIBlendStateDesc;
struct FRHIBlendState;
struct FRHIDepthStencilStateDesc;
struct FRHIDepthStencilState;
struct FUniformBufferPlainMember;
struct FUniformBufferResourceMember;
struct FRHIUniformBufferLayout;
struct FRHIUniformBuffer;
struct FRHIViewportDesc;
struct FRHIViewport;

using FRHIResourceRef = std::shared_ptr<FRHIResource>;

using FRHIBufferRef = std::shared_ptr<FRHIBuffer>;

using FRHIInputLayoutRef = std::shared_ptr<FRHIInputLayout>;

using FRHIShaderRef = std::shared_ptr<FRHIShader>;
using FRHIVertexShaderRef = std::shared_ptr<FRHIVertexShader>;
using FRHIPixelShaderRef = std::shared_ptr<FRHIPixelShader>;
using FRHIComputeShaderRef = std::shared_ptr<FRHIComputeShader>;
using FRHIGeometryShaderRef = std::shared_ptr<FRHIGeometryShader>;

using FRHITextureRef = std::shared_ptr<FRHITexture>;
using FRHISamplerStateRef = std::shared_ptr<FRHISamplerState>;
using FRHIRasterizerStateRef = std::shared_ptr<FRHIRasterizerState>;
using FRHIBlendStateRef = std::shared_ptr<FRHIBlendState>;

using FRHIDepthStencilStateRef = std::shared_ptr<FRHIDepthStencilState>;
using FRHIUniformBufferRef = std::shared_ptr<FRHIUniformBuffer>;
using FRHIViewportRef = std::shared_ptr<FRHIViewport>;

struct FRHIRenderPassInfo;
struct FAttribute;
} // namespace RHI
