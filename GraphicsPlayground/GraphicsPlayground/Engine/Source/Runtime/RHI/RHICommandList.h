#pragma once

#include "RHI/RHIFwd.h"
#include "Math/Color.h"
#include "RHI/RHIDefinitions.h"
#include "Container/Array.h"

namespace RHI
{
class FRHICommandList
{
public:
    virtual ~FRHICommandList() = default;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;

    // Resource Creation
    virtual FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Desc, const void* InitialData) = 0;
    virtual FRHIInputLayoutRef CreateInputLayout(const TArray<FAttribute>& Attributes, const FRHIVertexShaderRef& VertexShader) = 0;
    virtual FRHIVertexShaderRef CreateVertexShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHIPixelShaderRef CreatePixelShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHIComputeShaderRef CreateComputeShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHIGeometryShaderRef CreateGeometryShader(const FRHIShaderDesc& Desc) = 0;
    virtual FRHITextureRef CreateTexture(const FRHITextureDesc& Desc, const void* InitialData) = 0;
    virtual FRHISamplerStateRef CreateSamplerState(const FRHISamplerStateDesc& Desc) = 0;
    virtual FRHIRasterizerStateRef CreateRasterizerState(const FRHIRasterizerStateDesc& Desc) = 0;
    virtual FRHIBlendStateRef CreateBlendState(const FRHIBlendStateDesc& Desc) = 0;
    virtual FRHIDepthStencilStateRef CreateDepthStencilState(const FRHIDepthStencilStateDesc& Desc) = 0;
    virtual FRHIUniformBufferRef CreateUniformBuffer(const FRHIUniformBufferLayout* Layout, EUniformBufferUsage Usage) = 0;
    virtual FRHIViewportRef CreateViewport(const FRHIViewportDesc& Desc) = 0;

    // Render Passes
    virtual void BeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* Name) = 0;
    virtual void EndRenderPass() = 0;

    // Input Assembly
    virtual void SetPrimitiveTopology(EPrimitiveType PrimitiveType) = 0;
    virtual void SetInputLayout(FRHIInputLayout* InputLayout) = 0;
    virtual void SetVertexBuffer(uint32 Slot, FRHIBuffer* VertexBuffer, uint32 Stride, uint32 Offset) = 0;
    virtual void SetIndexBuffer(uint32 Slot, FRHIBuffer* IndexBuffer, uint32 Offset) = 0; // Index buffer uses uint32

    // Shaders
    virtual void SetVertexShader(FRHIVertexShader* VertexShader) = 0;
    virtual void SetPixelShader(FRHIPixelShader* PixelShader) = 0;
    virtual void SetComputeShader(FRHIComputeShader* ComputeShader) = 0;
    virtual void SetGeometryShader(FRHIGeometryShader* GeometryShader) = 0;

    virtual void SetStaticUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer) = 0;
    virtual void SetDynamicUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer) = 0;
    virtual void SetShaderResourceView(EShaderType TargetShader, FShaderResourceStaticSlot Slot, FRHIView* SRV) = 0;
    virtual void SetSampler(EShaderType TargetShader, FSamplerStaticSlot Slot, FRHISamplerState* SamplerState) = 0;

    // Rasterizer
    virtual void SetRasterizerState(FRHIRasterizerState* RasterizerState) = 0;
    virtual void SetBlendState(FRHIBlendState* BlendState, const FLinearColor& BlendFactor, uint32 SampleMask) = 0;
    virtual void SetDepthStencilState(FRHIDepthStencilState* DepthStencilState, uint32 StencilRef) = 0;
    virtual void SetViewport(FRHIViewport* Viewport) = 0;

    // Output Merger
    virtual void SetRenderTargets(uint32 NumRTVs, FRHIView* const* RTVs, FRHIView* DSV) = 0;

    // Updates
    virtual void UpdateBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Size) = 0;
    virtual void UpdateTexture(FRHITexture* Texture, const void* Data, uint32 Size) = 0;
    virtual void UpdateViewport(FRHIViewport* Viewport, const FRHIViewportDesc& Desc) = 0;

    // Clear
    /*
    * Note that BeginRenderPass() clears the render targets if specified in FRHIRenderPassInfo.
    */
    virtual void ClearColorTexture(FRHITexture* Texture, const FLinearColor& ClearColor) = 0;
    virtual void ClearDepthTexture(FRHITexture* Texture, float Depth, uint8 Stencil) = 0;

    // Draw
    virtual void DrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances) = 0;
    virtual void DrawIndexedPrimitive(int32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances) = 0;
};
} // namespace RHI
