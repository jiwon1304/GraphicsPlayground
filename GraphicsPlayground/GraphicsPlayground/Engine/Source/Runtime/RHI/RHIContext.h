#pragma once

#include "RHI/RHIFwd.h"
#include "RHI/RHIResources.h"

class FRHIShaderBindingLayout;

/** Static uniform buffers are used over all shaders */
class FUniformBufferStaticBindings
{
public:
	FUniformBufferStaticBindings() = default;

	FUniformBufferStaticBindings(std::initializer_list<FRHIUniformBuffer*> InitList)
	{
		for (FRHIUniformBuffer* Buffer : InitList)
		{
			AddUniformBuffer(Buffer);
		}
	}

	inline void AddUniformBuffer(FRHIUniformBuffer* UniformBuffer)
    {
        assert(UniformBuffers.Num() < InlineUniformBufferCount);
        UniformBuffers.Add(UniformBuffer);
    }

	inline void TryAddUniformBuffer(FRHIUniformBuffer* UniformBuffer)
	{
		if (UniformBuffer)
		{
			AddUniformBuffer(UniformBuffer);
		}
	}

	int32 GetUniformBufferCount() const
	{
		return UniformBuffers.Num();
	}

	FRHIUniformBuffer* GetUniformBuffer(int32 Index) const
	{
		return UniformBuffers[Index];
	}

	FUniformBufferStaticSlot GetSlot(int32 Index) const
	{
		return Slots[Index];
	}

	int32 GetSlotCount() const
	{
		return SlotCount;
	}

	const FRHIShaderBindingLayout* GetShaderBindingLayout() const
	{
		return ShaderBindingLayout;
	}

    void Bind(TArray<FRHIUniformBuffer*>& Bindings) const;

private:
	static constexpr uint32 InlineUniformBufferCount = 8;

    /**
     * Slot numbers.
     * Slot layout does not correspond to the order of slot number.
     */
	TArray<FUniformBufferStaticSlot, TInlineAllocator<FUniformBufferStaticSlot, InlineUniformBufferCount>> Slots;
    // Actual data
	TArray<FRHIUniformBuffer*, TInlineAllocator<FRHIUniformBuffer*, InlineUniformBufferCount>> UniformBuffers;
    // Slot names
	const FRHIShaderBindingLayout* ShaderBindingLayout = nullptr;
	int32 SlotCount = 0;
};


/** Kind of an executor */
class IRHICommandContext
{
public:
    virtual ~IRHICommandContext() = default;
    // Render Passes
    /** For profiling */
    virtual void RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName) = 0;
    virtual void RHIEndRenderPass() = 0;

    /** 
     * Only sets RTV, not DSV.
     * This method begins drawing to the backbuffer.
     * Therefore, drawing commands beforehand should be done on a separate texture.
     */
	virtual void RHIBeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI) = 0;
	
    /**
     * Mainly present/swap the backbuffer
     */
    virtual void RHIEndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync) = 0;

    virtual void RHISetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset) = 0;

    /** Min is inclusive, Max is exclusive */
	virtual void RHISetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) = 0;

    /** Parameter will be PSOFallBack */
    virtual void RHISetGraphicsPipelineState(FRHIGraphicsPipelineState* GraphicsState, uint32 StencilRef) = 0;

    // Draw
    virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances) = 0;
    virtual void RHIDrawIndexedPrimitive(uint32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances) = 0;

protected:
    FRHIRenderPassInfo RenderPassInfo;
};

class IRHIComputeContext : public IRHICommandContext
{
public:
	virtual void RHISetStaticUniformBuffers(const FUniformBufferStaticBindings& InUniformBuffers) = 0;

	virtual void RHISetStaticUniformBuffer(FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer) = 0;

	virtual void RHISetUniformBufferDynamicOffset(FUniformBufferStaticSlot Slot, uint32 Offset) = 0;
};


FBoundShaderStateRHIRef RHICreateBoundShaderState(
    FRHIVertexDeclaration* VertexDeclaration,
    FRHIVertexShader* VertexShader,
    FRHIPixelShader* PixelShader,
    FRHIGeometryShader* GeometryShader
);

class IRHICommandContextPSOFallback : public IRHIComputeContext
{
public:
	/**
	 * Call this functions directly rather than using commandlist
	 */
	virtual void RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderState) = 0;
	virtual void RHISetDepthStencilState(FRHIDepthStencilState* NewState, uint32 StencilRef) = 0;
	virtual void RHISetRasterizerState(FRHIRasterizerState* NewState) = 0;
	virtual void RHISetBlendState(FRHIBlendState* NewState, const FLinearColor& BlendFactor) = 0;
	virtual void RHIEnableDepthBoundsTest(bool bEnable) = 0;
	virtual void RHISetComputeShader(FRHIComputeShader* ComputeShader) = 0;

    virtual void RHISetGraphicsPipelineState(FRHIGraphicsPipelineState* GraphicsState, uint32 StencilRef) override
	{
		FRHIGraphicsPipelineStateFallBack* FallbackGraphicsState = static_cast<FRHIGraphicsPipelineStateFallBack*>(GraphicsState);
        
        assert(FallbackGraphicsState);

        RHISetBoundShaderState(
            RHICreateBoundShaderState(
                FallbackGraphicsState->Initializer.BoundShaderState.VertexDeclarationRHI,
                FallbackGraphicsState->Initializer.BoundShaderState.VertexShaderRHI,
                FallbackGraphicsState->Initializer.BoundShaderState.PixelShaderRHI,
                FallbackGraphicsState->Initializer.BoundShaderState.GeometryShaderRHI
            ).GetReference()
        );

        RHISetDepthStencilState(FallbackGraphicsState->Initializer.DepthStencilState, StencilRef);
        RHISetRasterizerState(FallbackGraphicsState->Initializer.RasterizerState);
        RHISetBlendState(FallbackGraphicsState->Initializer.BlendState, FLinearColor::White);
	}
};
