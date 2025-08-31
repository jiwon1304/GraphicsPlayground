#pragma once
#include "RHIFwd.h"
#include "RHIDefinitions.h"
#include "Container/Array.h"

class FRHICommandList;

namespace RHI
{
class FDynamicRHI
{
public:
    virtual ~FDynamicRHI() = default;
    virtual void Init() = 0;
    virtual void Shutdown() = 0;

    //virtual void RHIEndFrame_RenderThread(FRHICommandListImmediate& RHICmdList);

    //virtual void RHIEndFrame(const FRHIEndFrameArgs& Args) = 0;

    virtual void RHITick(float DeltaTime) = 0;
    virtual void RHIExecuteCommandList(FRHICommandList* CmdList) = 0;

    // -------------------------------------------------------------
    // Resource Creation
    // -------------------------------------------------------------
    virtual FRHIBufferRef               CreateBuffer(const FRHIBufferDesc& Desc, const void* InitialData) = 0;
    virtual FRHIInputLayoutRef          CreateInputLayout(const TArray<FAttribute>& Attributes, const FRHIVertexShaderRef& VertexShader) = 0;
    virtual FRHIVertexShaderRef         CreateVertexShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHIPixelShaderRef          CreatePixelShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHIComputeShaderRef        CreateComputeShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHIGeometryShaderRef       CreateGeometryShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHITextureRef              CreateTexture(const FRHITextureDesc& Desc, const void* InitialData) = 0;
    virtual FRHISamplerStateRef         CreateSamplerState(const FRHISamplerStateDesc& Desc) = 0;
    virtual FRHIRasterizerStateRef      CreateRasterizerState(const FRHIRasterizerStateDesc& Desc) = 0;
    virtual FRHIBlendStateRef           CreateBlendState(const FRHIBlendStateDesc& Desc) = 0;
    virtual FRHIDepthStencilStateRef    CreateDepthStencilState(const FRHIDepthStencilStateDesc& Desc) = 0;
    virtual FRHIUniformBufferRef        CreateUniformBuffer(const FRHIUniformBufferLayout* Layout, EUniformBufferUsage Usage) = 0;
    virtual FRHIViewportRef             CreateViewport(const FRHIViewportDesc& Desc) = 0;

    // Update Uniform Buffer?
};

extern FDynamicRHI* GDynamicRHI;
} // namespace RHI
