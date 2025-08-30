#pragma once

#include <memory>
//#include "RHIResources.h"

template<typename T> using TSharedPtr = std::shared_ptr<T>; 

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

using FRHIResourceRef = TSharedPtr<FRHIResource>;

using FRHIBufferRef = TSharedPtr<FRHIBuffer>;

using FRHIInputLayoutRef = TSharedPtr<FRHIInputLayout>;

using FRHIShaderRef = TSharedPtr<FRHIShader>;
using FRHIVertexShaderRef = TSharedPtr<FRHIVertexShader>;
using FRHIPixelShaderRef = TSharedPtr<FRHIPixelShader>;
using FRHIComputeShaderRef = TSharedPtr<FRHIComputeShader>;
using FRHIGeometryShaderRef = TSharedPtr<FRHIGeometryShader>;

using FRHITextureRef = TSharedPtr<FRHITexture>;
using FRHISamplerStateRef = TSharedPtr<FRHISamplerState>;
using FRHIRasterizerStateRef = TSharedPtr<FRHIRasterizerState>;
using FRHIBlendStateRef = TSharedPtr<FRHIBlendState>;

using FRHIDepthStencilStateRef = TSharedPtr<FRHIDepthStencilState>;
using FRHIUniformBufferRef = TSharedPtr<FRHIUniformBuffer>;
using FRHIViewportRef = TSharedPtr<FRHIViewport>;

struct FRHIRenderPassInfo;
struct FAttribute;
} // namespace RHI
