#pragma once

#include "RHI/RHIFwd.h"
#include "RHI/RHIResources.h"

namespace RHI
{
/** Kind of an executor */
class IRHICommandContext
{
public:
    virtual ~IRHICommandContext() = default;
    // Render Passes
    /** For profiling */
    virtual void RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName) = 0;
    virtual void RHIEndRenderPass() = 0;

    // Input Assembly
    virtual void RHISetPrimitiveTopology(EPrimitiveType PrimitiveType) = 0;
    virtual void RHISetInputLayout(FRHIInputLayout* InputLayout) = 0;
    virtual void RHISetVertexBuffer(uint32 Slot, FRHIBuffer* VertexBuffer, uint32 Stride, uint32 Offset) = 0;
    virtual void RHISetIndexBuffer(uint32 Slot, FRHIBuffer* IndexBuffer, uint32 Offset) = 0; // Index buffer uses uint32

    // Shaders
    virtual void RHISetVertexShader(FRHIVertexShader* VertexShader) = 0;
    virtual void RHISetPixelShader(FRHIPixelShader* PixelShader) = 0;
    virtual void RHISetComputeShader(FRHIComputeShader* ComputeShader) = 0;
    virtual void RHISetGeometryShader(FRHIGeometryShader* GeometryShader) = 0;

    virtual void RHISetStaticUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer) = 0;
    virtual void RHISetDynamicUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer) = 0;
    virtual void RHISetShaderResourceView(EShaderType TargetShader, FShaderResourceStaticSlot Slot, FRHIView* SRV) = 0;
    virtual void RHISetSampler(EShaderType TargetShader, FSamplerStaticSlot Slot, FRHISamplerState* SamplerState) = 0;

    // Rasterizer
    virtual void RHISetRasterizerState(FRHIRasterizerState* RasterizerState) = 0;
    virtual void RHISetBlendState(FRHIBlendState* BlendState, const FLinearColor& BlendFactor, uint32 SampleMask) = 0;
    virtual void RHISetDepthStencilState(FRHIDepthStencilState* DepthStencilState, uint32 StencilRef) = 0;
    virtual void RHISetViewport(FRHIViewport* Viewport) = 0;

    // Output Merger
    virtual void RHISetRenderTargets(uint32 NumRTVs, FRHIView* const* RTVs, FRHIView* DSV) = 0;

    // Updates
    virtual void RHIUpdateBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Size) = 0;
    virtual void RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBuffer, const void* Data, uint32 Size) = 0;
    virtual void RHIUpdateTexture(FRHITexture* Texture, const void* Data, uint32 Size) = 0;
    virtual void RHIUpdateViewport(FRHIViewport* Viewport, const FRHIViewportDesc& Desc) = 0;

    // Clear
    /*
    * Note that BeginRenderPass() clears the render targets if specified in FRHIRenderPassInfo.
    */
    virtual void RHIClearColorTexture(FRHITexture* Texture, const FLinearColor& ClearColor) = 0;
    virtual void RHIClearDepthTexture(FRHITexture* Texture, float Depth, uint8 Stencil) = 0;

    // Draw
    virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances) = 0;
    virtual void RHIDrawIndexedPrimitive(uint32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances) = 0;
};
}