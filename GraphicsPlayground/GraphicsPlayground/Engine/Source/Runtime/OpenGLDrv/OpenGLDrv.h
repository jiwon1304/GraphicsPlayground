#pragma once

#include "OpenGLFwd.h"
#include "RHI/RHI.h"
#include "RHI/RHIContext.h"
#include "IOpenGLDynamicRHI.h"
// #include "OpenGLState.h"

class FOpenGLDynamicRHI : public IOpenGLDynamicRHI, public IRHICommandContext
{
    static inline FOpenGLDynamicRHI* Singleton = nullptr;

public:
    static inline FOpenGLDynamicRHI& Get() { return *Singleton; }

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
	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclaration, FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader, FRHIGeometryShader* GeometryShader);

    virtual FGraphicsPipelineStateRHIRef RHICreateGraphicsPipelineState(const FGraphicsPipelineStateInitializer& Initializer);

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

private:
    GLFWwindow* MainWindow = nullptr;

    FOpenGLRHIState CurrentState;

    EPrimitiveType PrimitiveType = PT_TriangleList;
};
