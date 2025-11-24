#pragma once

#include "OpenGLFwd.h"
#include "RHI/RHI.h"
#include "RHI/RHIContext.h"
#include "IOpenGLDynamicRHI.h"
#include "OpenGLResources.h"
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

    template<typename TRHIType>
	static auto* ResourceCast(TRHIType* Resource)
	{
#ifdef _MSC_VER
        return reinterpret_cast<typename TOpenGLResourceTraits<TRHIType>::TConcreteType*>(Resource);
#else
		return static_cast<typename TOpenGLResourceTraits<TRHIType>::TConcreteType*>(Resource);
#endif
    }

	static FOpenGLTexture* ResourceCast(FRHITexture* TextureRHI)
	{
		if (!TextureRHI)
		{
			return nullptr;
		}
		else
		{
			return static_cast<FOpenGLTexture*>(TextureRHI->GetTextureBaseRHI());
		}
	}

    /**
     * Large and common data for overall render passes
     */
	void BindUniformBuffer(EShaderType ShaderType, uint32 BufferIndex, FRHIUniformBuffer* BufferRHI);

    /**
     * small and specific data for one render pass
     * NOT USED
     */
	// void SetShaderParametersCommon(EShaderType ShaderType, const TArray<uint8>& InParametersData, TConstArrayView<FRHIShaderParameter> InParameters, TConstArrayView<FRHIShaderParameterResource> InResourceParameters);
	// void SetShaderUnbindsCommon(EShaderType ShaderType, TConstArrayView<FRHIShaderParameterUnbind> InUnbinds);

    virtual void RHIEndFrame_RenderThread(class FRHICommandListImmediate& RHICmdList);
    virtual void RHIEndFrame(uint64 FrameNumber);

    // -------------------------------------------------------------
    // Resource Creation
    // -------------------------------------------------------------
	virtual FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer);
	virtual FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer);
	virtual FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer);
	virtual FBlendStateRHIRef RHICreateBlendState(const FBlendStateInitializerRHI& Initializer);
	virtual FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements);
	virtual FPixelShaderRHIRef RHICreatePixelShader(const TArray<const uint8>& Code, const uint16 Hash);
	virtual FVertexShaderRHIRef RHICreateVertexShader(const TArray<const uint8>& Code, const uint16 Hash);
	virtual FGeometryShaderRHIRef RHICreateGeometryShader(const TArray<const uint8>& Code, const uint16 Hash);
	// virtual FComputeShaderRHIRef RHICreateComputeShader(const TArray<const uint8>& Code, const uint16 Hash);

	/**
	* Creates a staging buffer, which is memory visible to the cpu without any locking.
	* @return The new staging-buffer.
	*/
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

	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclarationRHI, FRHIVertexShader* VertexShaderRHI, FRHIPixelShader* PixelShaderRHI, FRHIGeometryShader* GeometryShaderRHI);

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

protected:
    // FOpenGL::Window* MainWindow = nullptr;

    struct FPlatformOpenGLDevice* PlatformDevice = nullptr;

    EPrimitiveType PrimitiveType = PT_TriangleList;

    void InitializeStateResource();
};
