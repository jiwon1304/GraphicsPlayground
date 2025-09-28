#pragma once

#include "OpenGLFwd.h"
#include "RHI/RHI.h"
#include "RHI/RHIContext.h"
#include "IOpenGLDynamicRHI.h"
// #include "OpenGLState.h"

class FOpenGLDynamicRHI : public IOpenGLDynamicRHI, public IRHICommandContextPSOFallback
{
    static inline FOpenGLDynamicRHI* Singleton = nullptr;

public:
    static inline FOpenGLDynamicRHI& Get() { return *Singleton; }

    /**
     * FDyanmicRHI overrides
     */
    FOpenGLDynamicRHI();

    ~FOpenGLDynamicRHI();

    virtual void Init();
    virtual void Shutdown();

    virtual void RHIEndFrame_RenderThread(class FRHICommandListImmediate& RHICmdList);
    virtual void RHIEndFrame(uint64 FrameNumber);

    // -------------------------------------------------------------
    // Resource Creation
    // -------------------------------------------------------------
	// FlushType: Thread safe
	virtual FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer);

	// FlushType: Thread safe
	virtual FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer);

	// FlushType: Thread safe
	virtual FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer);

	// FlushType: Thread safe
	virtual FBlendStateRHIRef RHICreateBlendState(const FBlendStateInitializerRHI& Initializer);

	// FlushType: Wait RHI Thread
	virtual FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements);

	// FlushType: Wait RHI Thread
	virtual FPixelShaderRHIRef RHICreatePixelShader(/*TArrayView<const uint8> Code, const FSHAHash& Hash*/);

	// FlushType: Wait RHI Thread
	virtual FVertexShaderRHIRef RHICreateVertexShader(/*TArrayView<const uint8> Code, const FSHAHash& Hash*/);

	// FlushType: Wait RHI Thread
	virtual FGeometryShaderRHIRef RHICreateGeometryShader(/*TArrayView<const uint8> Code, const FSHAHash& Hash*/);

	// FlushType: Wait RHI Thread
	virtual FComputeShaderRHIRef RHICreateComputeShader(/*TArrayView<const uint8> Code, const FSHAHash& Hash*/);

	/**
	* Creates a staging buffer, which is memory visible to the cpu without any locking.
	* @return The new staging-buffer.
	*/
	// FlushType: Thread safe.	
	virtual FStagingBufferRHIRef RHICreateStagingBuffer()
	{
		return new FGenericRHIStagingBuffer();
	}

    /** 
     * Map to CPU memory
     * @param StagingBuffer : The staging buffer to lock
     * @param Offset : Offset into the staging buffer to lock
     * @param SizeRHI : Size of the region to lock
     * @return : Pointer to the locked region
     */
	virtual void* RHILockStagingBuffer(FRHIStagingBuffer* StagingBuffer, uint32 Offset, uint32 SizeRHI);

    virtual void RHIUnlockStagingBuffer(FRHIStagingBuffer* StagingBuffer);

    virtual void* LockStagingBuffer_RenderThread(class FRHICommmandListImmediate& RHICmdList, FRHIStagingBuffer* StagingBuffer, uint32 Offset, uint32 SizeRHI);

    virtual void UnlockStagingBuffer_RenderThread(class FRHICommmandListImmediate& RHICmdList, FRHIStagingBuffer* StagingBuffer);

	// FlushType: Thread safe, but varies depending on the RHI
	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclaration, FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader, FRHIGeometryShader* GeometryShader)
    {
        return nullptr;
    }

    // -------------------------------------------------------------
    // (Uniform) Buffers
    // -------------------------------------------------------------
    virtual FUniformBufferRHIRef RHICreateUniformBuffer(const void* Contents, const FRHIUniformBufferLayout* Layout, EUniformBufferUsage Usage);

    virtual void RHIUpdateUniformBuffer(FRHICommandListBase& RHICmdList, FRHIUniformBuffer* UniformBuffer, const void* Contents);

    // virtual FRHIBufferInitializer RHICreateBufferInitializer(FRHICommandListBase& RHICmdList, const FRHIBufferCreateDesc& CreateDesc);

	virtual FBufferRHIRef RHICreateBuffer(FRHICommandListBase& RHICmdList, FRHIBufferDesc const& Desc, ERHIAccess ResourceState, const void* InitialData);

    virtual void* RHILockBuffer(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer, uint32 Offset, uint32 Size);

	virtual void RHIUnlockBuffer(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer);

    virtual FTextureRHIRef RHICreateTexture(FRHICommandListBase& RHICmdList, const FRHITextureCreateDesc& CreateDesc, const void* InitialData);

    virtual void RHIUpdateTexture2D(FRHICommandListBase& RHICmdList, FRHITexture* Texture, uint32 MipIndex, const uint8* SourceData);

    virtual FShaderResourceViewRHIRef  RHICreateShaderResourceView (class FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc);
	virtual FUnorderedAccessViewRHIRef RHICreateUnorderedAccessView(class FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc);

    virtual FTextureRHIRef RHIGetViewportBackBuffer(FRHIViewport* Viewport);

	virtual FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat);

	virtual void RHIResizeViewport(FRHIViewport* Viewport, uint32 SizeX, uint32 SizeY, bool bIsFullscreen);

    // must be called from the main thread
    virtual void RHITick(float DeltaTime);

    virtual void RHISuspendRendering() {};

	virtual void RHIResumeRendering() {};

    virtual void* RHIGetNativeDevice();

	virtual IRHICommandContext* RHIGetDefaultContext();

    /**
     * FDynamicRHIPSOFallback overrides
     */
    // virtual FGraphicsPipelineStateRHIRef RHICreateGraphicsPipelineState(const FGraphicsPipelineStateInitializer& Initializer);

    /**
     * IRHICommandContextPSOFallback overrides
     */
    virtual void RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderState) override;
    virtual void RHISetDepthStencilState(FRHIDepthStencilState* NewState, uint32 StencilRef) override;
    virtual void RHISetBlendState(FRHIBlendState* NewState, const FLinearColor& BlendFactor) override;
	virtual void RHISetRasterizerState(FRHIRasterizerState* NewState) override;
	virtual void RHISetBlendState(FRHIBlendState* NewState, const FLinearColor& BlendFactor) override;
	virtual void RHIEnableDepthBoundsTest(bool bEnable) override;
	virtual void RHISetComputeShader(FRHIComputeShader* ComputeShader) override;

    /**
     * IRHIComputeContext overrides
     */
    virtual void RHISetStaticUniformBuffers(const FUniformBufferStaticBindings& InUniformBuffers) override;
	virtual void RHISetStaticUniformBuffer(FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer) override;
	virtual void RHISetUniformBufferDynamicOffset(FUniformBufferStaticSlot Slot, uint32 Offset) override;

    /**
     * IRHICommandContext overrides
     */
    virtual void RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName) override;
    virtual void RHIEndRenderPass() override;
	virtual void RHIBeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI) override;
    virtual void RHIEndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync) override;

    virtual void RHISetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset) override;
	virtual void RHISetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) override;
    virtual void RHISetGraphicsPipelineState(FRHIGraphicsPipelineState* GraphicsState, uint32 StencilRef) override;
    virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances) override;
    virtual void RHIDrawIndexedPrimitive(uint32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances) override;

private:
    FOpenGL::Window* MainWindow = nullptr;

    struct FPlatformOpenGLDevice* PlatformDevice = nullptr;

    EPrimitiveType PrimitiveType = PT_TriangleList;

    void InitializeStateResource();
};
