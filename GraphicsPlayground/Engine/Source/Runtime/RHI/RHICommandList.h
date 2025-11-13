#pragma once

#include "HAL/LinearAllocator.h"
#include "Container/Array.h"
#include "RHI/RHIFwd.h"
#include "Math/Color.h"
#include "RHI/RHIDefinitions.h"
#include "RHI/RHIContext.h"
#include "RHI/RHIBufferInitializer.h"
#include "RHI/DynamicRHI.h"

#ifndef RHI_COMMANDLIST_BYPASS
#define RHI_COMMANDLIST_BYPASS false
#endif

class FRHICommandListExecutor;
extern FRHICommandListExecutor GRHICommandList;

/*
* Classes which derives from FRHICommand have different class variables thus different sizes.
* Commands are accumulated and submitted at once. 
* For efficiency when accumulating, we does not use heap allocation for each command
* since the commands will not be allocated consequently. 
* Therefore, we use a linear allocator to allocate memory linearly for each command.
* Unlike std::vector or TArray, a linear allocator can allocate data with various sizes contiguously.
* 
* The rendering command pipeline is in such process :
* 1. Recording
* 2. Submitting
*/
struct FRHICommandBase
{
    FRHICommandBase* Next = nullptr;
    virtual void ExecuteAndDestruct(FRHICommandListBase& CmdList) = 0;
    virtual ~FRHICommandBase() = default;
};

/**
 * For execute arbitrary lambda function as a command
 * @tparam RHICmdListType : type which CmdList will be casted to when executing the lambda function
 * @tparam LAMBDA : lambda function type should take RHICmdListType& as parameter
 * @param InLambda : lambda function
 */
template <typename RHICmdListType, typename LAMBDA>
struct TRHILambdaCommand final : public FRHICommandBase
{
    LAMBDA Lambda;

    explicit TRHILambdaCommand(LAMBDA&& InLambda)
        : Lambda(std::forward<LAMBDA>(InLambda))
    {
    }

    void ExecuteAndDestruct(FRHICommandListBase& CmdList) override final
    {
        if constexpr (std::is_invocable_v<LAMBDA&, RHICmdListType&>)
        {
            Lambda(static_cast<RHICmdListType&>(CmdList));
        }
        else if constexpr (std::is_invocable_v<LAMBDA&>)
        {
            Lambda();
        }
        else
        {
            static_assert(std::is_invocable_v<LAMBDA&, RHICmdListType&> || std::is_invocable_v<LAMBDA&>,
                "LAMBDA must be callable with RHICmdListType& or with no parameters");
        }
        Lambda.~LAMBDA();
    }
};

/**
 * Base class for commands that can be added to the command list.
 * The actual command implementation is done with FRHICOMMAND_MACRO
 */
template <typename TCmd>
struct FRHICommand : public FRHICommandBase
{
    void ExecuteAndDestruct(FRHICommandListBase& CmdList) override final
    {
        static_cast<TCmd*>(this)->Execute(CmdList);
        static_cast<TCmd*>(this)->~TCmd(); // destruct the command object
    }
};

/**
 * Commands that inherits FRHICommand
 * This class carries arguments for the command
 * The actual execution is done by IRHICommandContext's API-specific derived class
 */
#define FRHICOMMAND_MACRO(CommandName) \
struct CommandName final : public FRHICommand<CommandName>

FRHICOMMAND_MACRO(FRHICommandBeginRenderPass)
{
    FRHIRenderPassInfo Info;
    const TCHAR* Name;
    FORCEINLINE_DEBUGGABLE FRHICommandBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName)
    : Info(InInfo), Name(InName)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandEndRenderPass)
{
    FORCEINLINE_DEBUGGABLE FRHICommandEndRenderPass() {}
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandBeginDrawingViewport)
{
	FRHIViewport* Viewport;
	FRHITexture* RenderTargetRHI;

	FORCEINLINE_DEBUGGABLE FRHICommandBeginDrawingViewport(FRHIViewport* InViewport, FRHITexture* InRenderTargetRHI)
		: Viewport(InViewport)
		, RenderTargetRHI(InRenderTargetRHI)
	{
	}
	FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandEndDrawingViewport)
{
    FRHIViewport* Viewport;
    bool bPresent;
    bool bLockToVsync;
    FORCEINLINE_DEBUGGABLE FRHICommandEndDrawingViewport(FRHIViewport* InViewport, bool InPresent, bool InLockToVsync)
        : Viewport(InViewport)
        , bPresent(InPresent)
        , bLockToVsync(InLockToVsync)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetStreamSource)
{
    uint32 StreamIndex;
    FRHIBuffer* VertexBuffer;
    uint32 Offset;

    FORCEINLINE_DEBUGGABLE FRHICommandSetStreamSource(uint32 InStreamIndex, FRHIBuffer* InVertexBuffer, uint32 InOffset)
        : StreamIndex(InStreamIndex)
        , VertexBuffer(InVertexBuffer)
        , Offset(InOffset)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetViewport)
{
    float MinX;
    float MinY;
    float MinZ;
    float MaxX;
    float MaxY;
    float MaxZ;

    FORCEINLINE_DEBUGGABLE FRHICommandSetViewport(float InMinX, float InMinY, float InMinZ, float InMaxX, float InMaxY, float InMaxZ)
        : MinX(InMinX)
        , MinY(InMinY)
        , MinZ(InMinZ)
        , MaxX(InMaxX)
        , MaxY(InMaxY)
        , MaxZ(InMaxZ)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetGraphicsPipelineState)
{
    FRHIGraphicsPipelineState* PipelineState;
    uint32 StencilRef;
    FORCEINLINE_DEBUGGABLE FRHICommandSetGraphicsPipelineState(FRHIGraphicsPipelineState* InPipelineState, uint32 InStencilRef)
        : PipelineState(InPipelineState)
        , StencilRef(InStencilRef)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandDrawPrimitive)
{
    uint32 BaseVertexIndex;
    uint32 NumVertices;
    uint32 NumInstances;
    FORCEINLINE_DEBUGGABLE FRHICommandDrawPrimitive(uint32 InBaseVertexIndex, uint32 InNumVertices, uint32 InNumInstances)
        : BaseVertexIndex(InBaseVertexIndex)
        , NumVertices(InNumVertices)
        , NumInstances(InNumInstances)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandDrawIndexedPrimitive)
{
    uint32 BaseVertexIndex;
    uint32 StartIndex;
    uint32 NumIndices;
    uint32 NumInstances;
    FORCEINLINE_DEBUGGABLE FRHICommandDrawIndexedPrimitive(uint32 InBaseVertexIndex, uint32 InStartIndex, uint32 InNumIndices, uint32 InNumInstances)
        : BaseVertexIndex(InBaseVertexIndex)
        , StartIndex(InStartIndex)
        , NumIndices(InNumIndices)
        , NumInstances(InNumInstances)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetStaticUniformBuffers)
{
    FUniformBufferStaticBindings UniformBuffers;
    FORCEINLINE_DEBUGGABLE FRHICommandSetStaticUniformBuffers(const FUniformBufferStaticBindings& InUniformBuffers)
        : UniformBuffers(InUniformBuffers)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetStaticUniformBuffer)
{
    FRHIUniformBuffer* Buffer;
    FUniformBufferStaticSlot Slot;
    FORCEINLINE_DEBUGGABLE FRHICommandSetStaticUniformBuffer(FUniformBufferStaticSlot InSlot, FRHIUniformBuffer* InBuffer)
        : Buffer(InBuffer)
        , Slot(InSlot)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetUniformBufferDynamicOffset)
{
    FUniformBufferStaticSlot Slot;
    uint32 Offset;
    FORCEINLINE_DEBUGGABLE FRHICommandSetUniformBufferDynamicOffset(FUniformBufferStaticSlot InSlot, uint32 InOffset)
        : Slot(InSlot)
        , Offset(InOffset)
    {
    }
    FORCEINLINE_DEBUGGABLE void Execute(FRHICommandListBase& CmdList);
};



/**
* Allocate Command. Place the command (starts with FRHICommand...) in __VA_ARGS__
* Sometimes commas in template <> breaks the macro. We use __VA_ARGS__ instead. 
* Example : ALLOC_COMMAND(FRHICommandSetStaticUniformBuffers)(UniformBuffers)
*                         ^ Derived class from FRHICommand    ^ Arguments
*/
#define ALLOC_COMMAND(...) \
new ( AllocCommand(sizeof(__VA_ARGS__), alignof(__VA_ARGS__)) ) \
__VA_ARGS__

class FRHICommandListBase
{
    friend class FRHICommandListExecutor;
public:
    FRHICommandListBase()
        : CommandLink(&Root)
    {
    }

    FRHICommandListBase(FRHICommandListBase const&) = delete;
    FRHICommandListBase& operator=(FRHICommandListBase const&) = delete;

    /** Finish recording commands and dispatch to the RHI thread */
    void FinishRecording();

    FORCEINLINE void* Alloc(size_t AllocSize, size_t Alignment)
    {
        return Allocator.Alloc(AllocSize, Alignment);
    }
    
    FORCEINLINE void* AllocCommand(size_t Size, size_t Alignment)
    {
        /**
         * **CommandLink        FRHICommand 객체 (value = 123, add 0xA)
         *                            ^
         *  *CommandLink        FRHICommand 객체의 메모리 주소 (val = 0xA, add = 0xB)
         *                            ^
         *   CommandLink        FRHICommand 객체의 메모리 주소가 담긴 메모리의 주소 (value = 0xB, add = 0xC)
         */
        /*
         * CommandLink는 항상 마지막 커맨드(Cmd1)의 Next(얘도 주소)의 주소를 갖고있음 (가리킴) = 새롭게 할당해야하는 주소 (아직 alloc 안됨)
         * 다시말해, 마지막 커맨드(Cmd1)의 멤버 변수이면서 타입이 FRHICommandBase*인 객체를 가리키고 있음
         * *CommandLink에 새로운 FRHICommandBase* 객체를 할당할 수 있음 -> 마지막 커맨드(Cmd1)의 멤버 변수 Next가 가리키는 값을 할당함
         * 다시말해서 새로운 커맨드의 주소를 할당할 수 있음.
         * 
         * *CommandLink = Result; 에서 마지막 커맨드(Cmd1)의 멤버 변수 Next에 다음 커맨드(Cmd2)의 주소가 할당이 됨
         * 이제 Cmd1->Next == Cmd2 (타입은 FRHICommandBase*)
         *
         * CommandLink = &Result->Next; 새로운 커맨드(Cmd2)의 멤버 변수 Next의 주소를 CommandLink에 할당
         * = 다음 커맨드의 주소를 적을 메모리 주소를 적음.
         */
        // 새롭게 할당된 메모리 주소
        FRHICommandBase* Result = static_cast<FRHICommandBase*>(Allocator.Alloc(Size, Alignment));
        
        // *CommandLink에 저장된 값 = 새로운 FRHICommand 객체의 메모리 주소 = placement new
        *CommandLink = Result;

        // "Result->Next" : 다음 FRHICommand 객체의 메모리 주소
        CommandLink = &Result->Next;
        
        ++NumCommands;

        // return the allocated memory for placement new
        return Result;
    }

    template <typename TCmd>
    FORCEINLINE void* AllocCommand()
    {
        return AllocCommand(sizeof(TCmd), alignof(TCmd));
    }
    
    template <typename LAMBDA>
    FORCEINLINE void EnqueueLambda(LAMBDA&& Fn)
    {
        ALLOC_COMMAND(TRHILambdaCommand<FRHICommandListBase, LAMBDA>)(std::forward<LAMBDA>(Fn));
    }
    
    bool HasCommands() const { return Root != nullptr; }
    uint32_t GetNumCommands() const { return NumCommands; }
    
    FORCEINLINE IRHICommandContext& GetContext()
    {
        return *GraphicsContext;
    }

    FORCEINLINE IRHIComputeContext& GetComputeContext()
    {
        return *ComputeContext;
    }

    // TODO : not using commandlist yet. Change when implementing command list
    FORCEINLINE bool Bypass() const { return RHI_COMMANDLIST_BYPASS; } 

    /** 
     * Initialize with TArray
     * @param Size Size of the buffer memory in bytes
     * @param Usage Usage flags for this buffer
     * @param Stride Stride between elements (only needed for structured buffers)
     * @param ResourceState How to read/write (= access) the resource
     * @param InitialData Initial data to populate the buffer with
     */
    FORCEINLINE FBufferRHIRef CreateBuffer(uint32 Size, EBufferUsageFlags Usage, uint32 Stride, ERHIAccess ResourceState, const TArray<uint8> InitialData)
	{
        assert(Size > 0);
        assert(InitialData.Num() > 0);

		FRHIBufferDesc BufferDesc(Size, Stride, Usage);

		FBufferRHIRef Buffer = GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, InitialData.GetData());
		return Buffer;
	}

    /** 
     * Initialize with C-style array.
     * @param Size Size of the buffer memory in bytes.
     * @param Usage Usage flags for this buffer
     * @param Stride Stride between elements (only needed for structured buffers)
     * @param ResourceState How to read/write (= access) the resource
     * @param InitialData Initial data to populate the buffer with. The length is determined by parameter Size. Can be freed after this function call.
     */
    FORCEINLINE FBufferRHIRef CreateBuffer(uint32 Size, EBufferUsageFlags Usage, uint32 Stride, ERHIAccess ResourceState, const void* InitialData)
	{
        assert(Size > 0);
        assert(InitialData != nullptr);

		FRHIBufferDesc BufferDesc(Size, Stride, Usage);

		FBufferRHIRef Buffer = GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, InitialData);
		return Buffer;
	}

    /**
     * Initialize without initial data
     * @param Size Size of the buffer memory in bytes
     * @param Usage Usage flags for this buffer
     * @param Stride Stride between elements (only needed for structured buffers)
     * @param ResourceState How to read/write (= access) the resource
     */
    FORCEINLINE FBufferRHIRef CreateBuffer(uint32 Size, EBufferUsageFlags Usage, uint32 Stride, ERHIAccess ResourceState)
    {
        assert(Size > 0);

        FRHIBufferDesc BufferDesc(Size, Stride, Usage);

        FBufferRHIRef Buffer = GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, nullptr);
        return Buffer;
    }

    /**
     * Vertex Buffer
     */

     /**
      * Create Vertex Buffer with explicit resource state
      * @param Size Size of the buffer memory in bytes
      * @param Usage Usage flags for this buffer
      * @param ResourceState How to read/write (= access) the resource
      * @param InitialData Initial data to populate the buffer with
      */
    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, const TArray<uint8> InitialData)
    {
        return CreateBuffer(Size, Usage | EBufferUsageFlags::VertexBuffer, 0, ResourceState, InitialData.GetData());
    }
    
    /**
     * Create Vertex Buffer with explicit resource state
     * @param Size Size of the buffer memory in bytes
     * @param Usage Usage flags for this buffer
     * @param ResourceState How to read/write (= access) the resource
     * @param InitialData Initial data to populate the buffer with. The length is determined by parameter Size.
     */
    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, const void* InitialData)
    {
        return CreateBuffer(Size, Usage | EBufferUsageFlags::VertexBuffer, 0, ResourceState, InitialData);
    }

    /**
     * Create Vertex Buffer with default resource state (= EBufferUsageFlags::VertexBuffer)
     * @param Size Size of the buffer memory in bytes
     * @param Usage Usage flags for this buffer
     * @param InitialData Initial data to populate the buffer with
     */
    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, const TArray<uint8> InitialData)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::VertexBuffer);
        return CreateVertexBuffer(Size, Usage, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, const void* InitialData)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::VertexBuffer);
        return CreateVertexBuffer(Size, Usage, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::VertexBuffer);
        return CreateBuffer(Size, Usage, 0, ResourceState);
    }

    /**
     * Index Buffer
     */
    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, const TArray<uint8> InitialData)
    {
        return CreateBuffer(Size, Usage | EBufferUsageFlags::IndexBuffer, 0, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, const void* InitialData)
    {
        return CreateBuffer(Size, Usage | EBufferUsageFlags::IndexBuffer, 0, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage, const TArray<uint8> InitialData)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::IndexBuffer);
        return CreateIndexBuffer(Size, Usage, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage, const void* InitialData)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::IndexBuffer);
        return CreateIndexBuffer(Size, Usage, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, EBufferUsageFlags Usage)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::IndexBuffer);
        return CreateBuffer(Size, Usage, 0, ResourceState);
    }

    /**
     * Structured Buffer
     */
	FORCEINLINE FBufferRHIRef CreateStructuredBuffer(uint32 Stride, uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, const TArray<uint8> InitialData)
	{
		return CreateBuffer(Size, Usage | EBufferUsageFlags::StructuredBuffer, Stride, ResourceState, InitialData);
	}

    FORCEINLINE FBufferRHIRef CreateStructuredBuffer(uint32 Stride, uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, const void* InitialData)
	{
		return CreateBuffer(Size, Usage | EBufferUsageFlags::StructuredBuffer, Stride, ResourceState, InitialData);
	}

    FORCEINLINE FBufferRHIRef CreateStructuredBuffer(uint32 Stride, uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState)
	{
		return CreateBuffer(Size, Usage | EBufferUsageFlags::StructuredBuffer, Stride, ResourceState);
	}

	FORCEINLINE FBufferRHIRef CreateStructuredBuffer(uint32 Stride, uint32 Size, EBufferUsageFlags Usage, const TArray<uint8> InitialData)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::StructuredBuffer);
        return CreateStructuredBuffer(Stride, Size, Usage, ResourceState, InitialData);
    }

	FORCEINLINE FBufferRHIRef CreateStructuredBuffer(uint32 Stride, uint32 Size, EBufferUsageFlags Usage, const void* InitialData)
    {
        ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::StructuredBuffer);
        return CreateStructuredBuffer(Stride, Size, Usage, ResourceState, InitialData);
    }

    FORCEINLINE FBufferRHIRef CreateStructuredBuffer(uint32 Stride, uint32 Size, EBufferUsageFlags Usage)
	{
		ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::StructuredBuffer);
		return CreateStructuredBuffer(Stride, Size, Usage, ResourceState);
	}

    /**
     * Uniform Buffer
     * Use CreateBuffer to create a buffer for uniform buffer.
     */
    FORCEINLINE void UpdateUniformBuffer(FRHIUniformBuffer* UniformBufferRHI, const void* Contents)
    {
        GDynamicRHI->RHIUpdateUniformBuffer(*this, UniformBufferRHI, Contents);
    }

    /**
     * Stream Source Slot (layout of vertex buffer)
     * Update buffer in parameter "StreamSourceSlotRHI" with "BufferRHI"
     * If bypass is true, update immediately
     */
    FORCEINLINE void UpdateStreamSourceSlot(FRHIStreamSourceSlot* StreamSourceSlotRHI, FRHIBuffer* BufferRHI)
    {
        EnqueueLambda([=](FRHICommandListBase& CmdList)
        {
            StreamSourceSlotRHI->Buffer = BufferRHI;
        });
    }

    /**
     * Texture
     */
	FORCEINLINE FTextureRHIRef CreateTexture(const FRHITextureCreateDesc& CreateDesc)
	{
		return GDynamicRHI->RHICreateTexture(*this, CreateDesc, nullptr);
	}

    FORCEINLINE void UpdateTexture2D(FRHITexture* Texture, uint32 MipIndex, const uint8* SourceData)
    {
        GDynamicRHI->RHIUpdateTexture2D(*this, Texture, MipIndex, SourceData);
    }

    /**
     * Shader Resource View (SRV) / Unordered Access View (UAV)
     */
	FORCEINLINE FShaderResourceViewRHIRef CreateShaderResourceView(FRHIBuffer* Buffer, FRHIViewDesc::FBufferSRV::FInitializer const& ViewDesc)
	{
		return GDynamicRHI->RHICreateShaderResourceView(*this, Buffer, ViewDesc);
	}

	FORCEINLINE FShaderResourceViewRHIRef CreateShaderResourceView(FRHITexture* Texture, FRHIViewDesc::FTextureSRV::FInitializer const& ViewDesc)
	{
		return GDynamicRHI->RHICreateShaderResourceView(*this, Texture, ViewDesc);
	}

    FORCEINLINE FUnorderedAccessViewRHIRef CreateUnorderedAccessView(FRHIBuffer* Buffer, FRHIViewDesc::FBufferUAV::FInitializer const& ViewDesc)
	{
		return GDynamicRHI->RHICreateUnorderedAccessView(*this, Buffer, ViewDesc);
	}

	FORCEINLINE FUnorderedAccessViewRHIRef CreateUnorderedAccessView(FRHITexture* Texture, FRHIViewDesc::FTextureUAV::FInitializer const& ViewDesc)
	{
		return GDynamicRHI->RHICreateUnorderedAccessView(*this, Texture, ViewDesc);
	}

private:
    friend class FRHICommandListExecutor;
    // Execute ----------------------------------------------------
    void Execute()
    {
        FRHICommandBase* Node = Root;
        while (Node)
        {
            FRHICommandBase* Next = Node->Next;
            Node->ExecuteAndDestruct(*this);
            Node = Next;
        }
        Root = nullptr;
        CommandLink = &Root;
        NumCommands = 0;
    }

    // Execute + Reset the offset
    void ExecuteAndReset()
    {
        Execute();
        Allocator.Reset();
    }

protected:
    IRHICommandContext* GraphicsContext = nullptr;
    IRHIComputeContext* ComputeContext = nullptr;
    
    /**
     * pointer to the first command in the linked list.
     */
    FRHICommandBase* Root = nullptr;

    /**
     * pointer to the "Next" member variable of the last command in the linked list.
     */
    FRHICommandBase** CommandLink = nullptr;
    uint32_t           NumCommands = 0;

    /**
     * @todo : make Allocator fits into page size
     */
    FLinearAllocator   Allocator;
};

class IRHICommandContext;

class FRHICommandList : public FRHICommandListBase
{
    friend class FRHICommandListExecutor;
public:
    virtual ~FRHICommandList() = default;

    template <typename LAMBDA>
    FORCEINLINE void EnqueueLambda(LAMBDA&& Fn)
    {
        ALLOC_COMMAND(TRHILambdaCommand<FRHICommandList, LAMBDA>)(std::forward<LAMBDA>(Fn));
    }
  
    void BeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* Name)
    {
        if (Bypass())
        {
            GetContext().RHIBeginRenderPass(InInfo, Name);
        }
        else
        {
            ALLOC_COMMAND(FRHICommandBeginRenderPass)(InInfo, Name);
        }
    }

    void EndRenderPass()
    {
        if (Bypass())
        {
            GetContext().RHIEndRenderPass();
        }
        else
        {
            ALLOC_COMMAND(FRHICommandEndRenderPass)();
        }
    }
};

/** 
 * We currently use FRHICommandList as an immediate list.
 * We do not have any deferred threads.
 */
class FRHICommandListImmediate : public FRHICommandList
{
    friend class FRHICommandListExecutor;

public:
    virtual ~FRHICommandListImmediate() override = default;

    template <typename LAMBDA>
    FORCEINLINE void EnqueueLambda(LAMBDA&& Fn)
    {
        ALLOC_COMMAND(TRHILambdaCommand<FRHICommandListImmediate, LAMBDA>)(std::forward<LAMBDA>(Fn));
    }

    static FRHICommandListImmediate& Get();

    void BeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI)
    {
        if (Bypass())
        {
            GetContext().RHIBeginDrawingViewport(Viewport, RenderTargetRHI);
        }
        else
        {
            ALLOC_COMMAND(FRHICommandBeginDrawingViewport)(Viewport, RenderTargetRHI);
        }
    }
    void EndDrawingViewport(FRHIViewport* Viewport, bool bPresent)
    {
        if (Bypass())
        {
            GetContext().RHIEndDrawingViewport(Viewport, bPresent, false);
        }
        else
        {
            ALLOC_COMMAND(FRHICommandEndDrawingViewport)(Viewport, bPresent, false);
        }
    }

    // Flush to GPU and waits until all commands are executed
    void ImmediateFlush();
};

/**
 * Executor for RHICommandList
 */
class FRHICommandListExecutor
{
public:
    static inline FRHICommandListImmediate& GetImmediateCommandList()
    {
        return GRHICommandList.CommandListImmediate;
    }

    /**
     * Dispatch and empty the command list
     */
    void Submit();

    friend class FRHICommandListBase;
    friend class FRHICommandListImmediate;

    // We now only use FRHICommandListImmediate. No other command lists.
    FRHICommandListImmediate CommandListImmediate;
};


#include "RHICommandListCommandExecutes.inl"
