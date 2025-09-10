#pragma once

#include "OpenGLFwd.h"
#include "RHI.h"
#include "RHIContext.h"
#include "IOpenGLDynamicRHI.h"
#include "OpenGLState.h"

class FOpenGLDynamicRHI : public IOpenGLDynamicRHI, public IRHICommandContext
{
    static inline FOpenGLDynamicRHI* Singleton = nullptr;
    static inline FOpenGLDynamicRHI* Singleton = nullptr;

public:
    static inline FOpenGLDynamicRHI& Get() { return *Singleton; }

    FOpenGLDynamicRHI();

    ~FOpenGLDynamicRHI();

    virtual void Init() override;

    virtual void Shutdown() override;

    virtual void RHITick(float DeltaTime) override;
    
    virtual void RHIExecuteCommandList(FRHICommandList* CmdList) override;

    // Render Passes
    /** For profiling */
    virtual void RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName) = 0;
    virtual void RHIEndRenderPass() = 0;

    // Input Assembly
    virtual void RHISetPrimitiveTopology(EPrimitiveType PrimitiveType);
    virtual void RHISetInputLayout(FRHIVertexDeclaration* InputLayout);
    virtual void RHISetVertexBuffer(uint32 Slot, FRHIBuffer* VertexBuffer, uint32 Stride, uint32 Offset);
    virtual void RHISetIndexBuffer(uint32 Slot, FRHIBuffer* IndexBuffer, uint32 Offset); // Index buffer uses uint32

    // Shaders
    virtual void RHISetVertexShader(FRHIVertexShader* VertexShader);
    virtual void RHISetPixelShader(FRHIPixelShader* PixelShader);
    virtual void RHISetComputeShader(FRHIComputeShader* ComputeShader);
    virtual void RHISetGeometryShader(FRHIGeometryShader* GeometryShader);

    virtual void RHISetStaticUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer);
    virtual void RHISetDynamicUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer);
    virtual void RHISetShaderResourceView(EShaderType TargetShader, FShaderResourceStaticSlot Slot, FRHIView* SRV);
    virtual void RHISetSampler(EShaderType TargetShader, FSamplerStaticSlot Slot, FRHISamplerState* SamplerState);

    // Rasterizer
    virtual void RHISetRasterizerState(FRHIRasterizerState* RasterizerState);
    virtual void RHISetBlendState(FRHIBlendState* BlendState, const FLinearColor& BlendFactor, uint32 SampleMask);
    virtual void RHISetDepthStencilState(FRHIDepthStencilState* DepthStencilState, uint32 StencilRef);
    virtual void RHISetViewport(FRHIViewport* Viewport);

    // Output Merger
    virtual void RHISetRenderTargets(uint32 NumRTVs, FRHIView* const* RTVs, FRHIView* DSV);

    // Updates
    virtual void RHIUpdateBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Size);
    virtual void RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBuffer, const void* Data, uint32 Size);
    virtual void RHIUpdateTexture(FRHITexture* Texture, const void* Data, uint32 Size);
    virtual void RHIUpdateViewport(FRHIViewport* Viewport, const FRHIViewportDesc& Desc);

    // Clear
    /*
    * Note that BeginRenderPass() clears the render targets if specified in FRHIRenderPassInfo.
    */
    virtual void RHIClearColorTexture(FRHITexture* Texture, const FLinearColor& ClearColor);
    virtual void RHIClearDepthTexture(FRHITexture* Texture, float Depth, uint8 Stencil);

    // Draw
    virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances) = 0;
    virtual void RHIDrawIndexedPrimitive(uint32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances) = 0;

private:
    GLFWwindow* MainWindow = nullptr;

    FOpenGLRHIState CurrentState;
};
