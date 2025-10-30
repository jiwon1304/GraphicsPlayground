#pragma once
#include "RHIFwd.h"
#include "RHIDefinitions.h"
#include "RHIAccess.h"
#include "PixelFormat.h"
#include "Container/Array.h"

class FRHICommandListBase;
class IRHICommandContext;

/** Create resources, like device in D3D11 */
class FDynamicRHI
{
public:
    virtual ~FDynamicRHI() = default;
    virtual void Init() = 0;
    virtual void Shutdown() = 0;

    virtual void RHIEndFrame_RenderThread(class FRHICommandListImmediate& RHICmdList);
    virtual void RHIEndFrame(uint64 FrameNumber) = 0;

    // -------------------------------------------------------------
    // Resource Creation
    // -------------------------------------------------------------
	// FlushType: Thread safe
	virtual FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer) = 0;

	// FlushType: Thread safe
	virtual FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer) = 0;

	// FlushType: Thread safe
	virtual FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer) = 0;

	// FlushType: Thread safe
	virtual FBlendStateRHIRef RHICreateBlendState(const FBlendStateInitializerRHI& Initializer) = 0;

	// FlushType: Wait RHI Thread
	virtual FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements) = 0;

	// FlushType: Wait RHI Thread
	virtual FPixelShaderRHIRef RHICreatePixelShader(const TArray<const uint8>& Code) = 0;

	// FlushType: Wait RHI Thread
	virtual FVertexShaderRHIRef RHICreateVertexShader(const TArray<const uint8>& Code) = 0;

	// FlushType: Wait RHI Thread
	virtual FGeometryShaderRHIRef RHICreateGeometryShader(const TArray<const uint8>& Code) = 0;

	// FlushType: Wait RHI Thread
	virtual FComputeShaderRHIRef RHICreateComputeShader(const TArray<const uint8>& Code) = 0;

	/**
	* Creates a staging buffer, which is memory visible to the cpu without any locking.
	* @return The new staging-buffer.
	*/
	// FlushType: Thread safe.	
	// virtual FStagingBufferRHIRef RHICreateStagingBuffer()
	// {
	// 	return new FGenericRHIStagingBuffer();
	// }

    /** 
     * Map to CPU memory
     * @param StagingBuffer : The staging buffer to lock
     * @param Offset : Offset into the staging buffer to lock
     * @param SizeRHI : Size of the region to lock
     * @return : Pointer to the locked region
     */
	virtual void* RHILockStagingBuffer(FRHIStagingBuffer* StagingBuffer, uint32 Offset, uint32 SizeRHI);

    virtual void RHIUnlockStagingBuffer(FRHIStagingBuffer* StagingBuffer);

    virtual void* LockStagingBuffer_RenderThread(class FRHICommandListImmediate& RHICmdList, FRHIStagingBuffer* StagingBuffer, uint32 Offset, uint32 SizeRHI);

    virtual void UnlockStagingBuffer_RenderThread(class FRHICommandListImmediate& RHICmdList, FRHIStagingBuffer* StagingBuffer);

	// FlushType: Thread safe, but varies depending on the RHI
	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclaration, FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader, FRHIGeometryShader* GeometryShader) = 0;

    virtual FGraphicsPipelineStateRHIRef RHICreateGraphicsPipelineState(const FGraphicsPipelineStateInitializer& Initializer) = 0;

    // -------------------------------------------------------------
    // (Uniform) Buffers
    // -------------------------------------------------------------
    virtual FUniformBufferRHIRef RHICreateUniformBuffer(const void* Contents, const FRHIUniformBufferLayout* Layout, EUniformBufferUsage Usage) = 0;

    virtual void RHIUpdateUniformBuffer(FRHICommandListBase& RHICmdList, FRHIUniformBuffer* UniformBuffer, const void* Contents) = 0;

    // virtual FRHIBufferInitializer RHICreateBufferInitializer(FRHICommandListBase& RHICmdList, const FRHIBufferCreateDesc& CreateDesc) = 0;

	virtual FBufferRHIRef RHICreateBuffer(FRHICommandListBase& RHICmdList, FRHIBufferDesc const& Desc, ERHIAccess ResourceState, const void* InitialData) = 0;

    virtual void* RHILockBuffer(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer, uint32 Offset, uint32 Size) = 0;

	virtual void RHIUnlockBuffer(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer) = 0;

    virtual FTextureRHIRef RHICreateTexture(FRHICommandListBase& RHICmdList, const FRHITextureCreateDesc& CreateDesc, const void* InitialData) = 0;

    virtual void RHIUpdateTexture2D(FRHICommandListBase& RHICmdList, FRHITexture* Texture, uint32 MipIndex, const uint8* SourceData) = 0;

    virtual FShaderResourceViewRHIRef  RHICreateShaderResourceView (class FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc) = 0;
	virtual FUnorderedAccessViewRHIRef RHICreateUnorderedAccessView(class FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc) = 0;

    virtual FTextureRHIRef RHIGetViewportBackBuffer(FRHIViewport* Viewport) = 0;

	virtual FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat) = 0;

	virtual void RHIResizeViewport(FRHIViewport* Viewport, uint32 SizeX, uint32 SizeY, bool bIsFullscreen) = 0;

    // must be called from the main thread
    virtual void RHITick(float DeltaTime) = 0;

    virtual void RHISuspendRendering() {};

	virtual void RHIResumeRendering() {};

    virtual void* RHIGetNativeDevice() = 0;

	virtual IRHICommandContext* RHIGetDefaultContext() = 0;
};

class FDynamicRHIPSOFallback : public FDynamicRHI
{
public:
    virtual FGraphicsPipelineStateRHIRef RHICreateGraphicsPipelineState(const FGraphicsPipelineStateInitializer& Initializer) override;
};

extern FDynamicRHI* GDynamicRHI;

FORCEINLINE FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer)
{
    return GDynamicRHI->RHICreateSamplerState(Initializer);
}

FORCEINLINE FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer)
{
    return GDynamicRHI->RHICreateRasterizerState(Initializer);
}

FORCEINLINE FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer)
{
    return GDynamicRHI->RHICreateDepthStencilState(Initializer);
}

FORCEINLINE FBlendStateRHIRef RHICreateBlendState(const FBlendStateInitializerRHI& Initializer)
{
    return GDynamicRHI->RHICreateBlendState(Initializer);
}

FORCEINLINE FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements)
{
    return GDynamicRHI->RHICreateVertexDeclaration(Elements);
}

FORCEINLINE FPixelShaderRHIRef RHICreatePixelShader(const TArray<const uint8> &Code)
{
    return GDynamicRHI->RHICreatePixelShader(Code);
}

FORCEINLINE FVertexShaderRHIRef RHICreateVertexShader(const TArray<const uint8> &Code)
{
    return GDynamicRHI->RHICreateVertexShader(Code);
}

FORCEINLINE FGeometryShaderRHIRef RHICreateGeometryShader(const TArray<const uint8> &Code)
{
    return GDynamicRHI->RHICreateGeometryShader(Code);
}

FORCEINLINE FComputeShaderRHIRef RHICreateComputeShader(const TArray<const uint8> &Code)
{
    return GDynamicRHI->RHICreateComputeShader(Code);
}

// FORCEINLINE FStagingBufferRHIRef RHICreateStagingBuffer()
// {
//     return GDynamicRHI->RHICreateStagingBuffer();
// }

FORCEINLINE FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclaration, FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader, FRHIGeometryShader* GeometryShader)
{
    return GDynamicRHI->RHICreateBoundShaderState(VertexDeclaration, VertexShader, PixelShader, GeometryShader);
}

FORCEINLINE FGraphicsPipelineStateRHIRef RHICreateGraphicsPipelineState(const FGraphicsPipelineStateInitializer& Initializer)
{
    return GDynamicRHI->RHICreateGraphicsPipelineState(Initializer);
}

FORCEINLINE FUniformBufferRHIRef RHICreateUniformBuffer(const void* Contents, const FRHIUniformBufferLayout* Layout, EUniformBufferUsage Usage)
{
    return GDynamicRHI->RHICreateUniformBuffer(Contents, Layout, Usage);
}

FORCEINLINE void RHIUpdateUniformBuffer(FRHICommandListBase& RHICmdList, FRHIUniformBuffer* UniformBuffer, const void* Contents)
{
    return GDynamicRHI->RHIUpdateUniformBuffer(RHICmdList, UniformBuffer, Contents);
}

FORCEINLINE FBufferRHIRef RHICreateBuffer(FRHICommandListBase& RHICmdList, FRHIBufferDesc const& Desc, ERHIAccess ResourceState, const void* InitialData)
{
    return GDynamicRHI->RHICreateBuffer(RHICmdList, Desc, ResourceState, InitialData);
}

FORCEINLINE void* RHILockBuffer(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer, uint32 Offset, uint32 Size)
{
    return GDynamicRHI->RHILockBuffer(RHICmdList, Buffer, Offset, Size);
}

FORCEINLINE void RHIUnlockBuffer(FRHICommandListBase& RHICmdList, FRHIBuffer* Buffer)
{
    return GDynamicRHI->RHIUnlockBuffer(RHICmdList, Buffer);
}

FORCEINLINE FTextureRHIRef RHICreateTexture(FRHICommandListBase& RHICmdList, const FRHITextureCreateDesc& CreateDesc, const void* InitialData)
{
    return GDynamicRHI->RHICreateTexture(RHICmdList, CreateDesc, InitialData);
}

FORCEINLINE void RHIUpdateTexture2D(FRHICommandListBase& RHICmdList, FRHITexture* Texture, uint32 MipIndex, const uint8* SourceData)
{
    return GDynamicRHI->RHIUpdateTexture2D(RHICmdList, Texture, MipIndex, SourceData);
}

FORCEINLINE FShaderResourceViewRHIRef  RHICreateShaderResourceView (class FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc)
{
    return GDynamicRHI->RHICreateShaderResourceView(RHICmdList, Resource, ViewDesc);
}

FORCEINLINE FUnorderedAccessViewRHIRef RHICreateUnorderedAccessView(class FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc)
{
    return GDynamicRHI->RHICreateUnorderedAccessView(RHICmdList, Resource, ViewDesc);
}

FORCEINLINE FTextureRHIRef RHIGetViewportBackBuffer(FRHIViewport* Viewport)
{
    return GDynamicRHI->RHIGetViewportBackBuffer(Viewport);
}

FORCEINLINE FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat)
{
    return GDynamicRHI->RHICreateViewport(WindowHandle, SizeX, SizeY, bIsFullscreen, PreferredPixelFormat);
}

FORCEINLINE void RHIResizeViewport(FRHIViewport* Viewport, uint32 SizeX, uint32 SizeY, bool bIsFullscreen)
{
    return GDynamicRHI->RHIResizeViewport(Viewport, SizeX, SizeY, bIsFullscreen);
}
