#pragma once

#include <HAL/LinearAllocator.h>
#include "Container/Array.h"
#include "RHI/RHIFwd.h"
#include "Math/Color.h"
#include "RHI/RHIDefinitions.h"
#include "RHI/RHIContext.h"

#ifndef RHI_COMMANDLIST_BYPASS
#define RHI_COMMANDLIST_BYPASS false
#endif

namespace RHI
{
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
* Game thread queues the commands. E
* 2. Submitting
*/
struct FRHICommandBase
{
    FRHICommandBase* Next = nullptr;
    virtual void ExecuteAndDestruct(FRHICommandListBase& CmdList) = 0;
    virtual ~FRHICommandBase() = default;
};

/*
* For general purpose
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
        Lambda(static_cast<RHICmdListType&>(CmdList));
        this->~TRHILambdaCommand();
    }
};

template <typename TCmd>
struct FRHICommand : public FRHICommandBase
{
void ExecuteAndDestruct(FRHICommandListBase& CmdList) override final
    {
        static_cast<TCmd*>(this)->Execute(CmdList);
        this->~TCmd();
    }
};

/**
 * Commands that inherits FRHICommand
 * This class carries arguments for the command
 * The actual execution is done by IRHICommandContext's API-specific derived class
 */
#define FRHICOMMAND_MACRO(CommandName) \
struct CommandName final : public FRHICommand<CommandName>


// 1. Render Pass
FRHICOMMAND_MACRO(FRHICommandBeginRenderPass)
{
    FRHIRenderPassInfo Info;
    const TCHAR* Name;
    FRHICommandBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName)
        : Info(InInfo), Name(InName)
    {
    }
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandEndRenderPass)
{
    FRHICommandEndRenderPass() {}
    void Execute(FRHICommandListBase& CmdList);
};

// 2. Input Assembly
FRHICOMMAND_MACRO(FRHICommandSetPrimitiveTopology)
{
    EPrimitiveType PrimitiveType;
    FRHICommandSetPrimitiveTopology(EPrimitiveType InPrimitiveType)
        : PrimitiveType(InPrimitiveType) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetInputLayout)
{
    FRHIInputLayout* InputLayout;
    FRHICommandSetInputLayout(FRHIInputLayout* InInputLayout)
        : InputLayout(InInputLayout) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetVertexBuffer)
{
    uint32 Slot;
    FRHIBuffer* VertexBuffer;
    uint32 Stride;
    uint32 Offset;
    FRHICommandSetVertexBuffer(uint32 InSlot, FRHIBuffer* InVertexBuffer, uint32 InStride, uint32 InOffset)
        : Slot(InSlot), VertexBuffer(InVertexBuffer), Stride(InStride), Offset(InOffset) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetIndexBuffer)
{
    uint32 Slot;
    FRHIBuffer* IndexBuffer;
    uint32 Offset;
    FRHICommandSetIndexBuffer(uint32 InSlot, FRHIBuffer* InIndexBuffer, uint32 InOffset)
        : Slot(InSlot), IndexBuffer(InIndexBuffer), Offset(InOffset) {}
    void Execute(FRHICommandListBase& CmdList);
};

// 3. Shaders
FRHICOMMAND_MACRO(FRHICommandSetVertexShader)
{
    FRHIVertexShader* Shader;
    FRHICommandSetVertexShader(FRHIVertexShader* InShader) : Shader(InShader) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetPixelShader)
{
    FRHIPixelShader* Shader;
    FRHICommandSetPixelShader(FRHIPixelShader* InShader) : Shader(InShader) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetComputeShader)
{
    FRHIComputeShader* Shader;
    FRHICommandSetComputeShader(FRHIComputeShader* InShader) : Shader(InShader) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetGeometryShader)
{
    FRHIGeometryShader* Shader;
    FRHICommandSetGeometryShader(FRHIGeometryShader* InShader) : Shader(InShader) {}
    void Execute(FRHICommandListBase& CmdList);
};

// 4. Shader Resources / Uniforms
FRHICOMMAND_MACRO(FRHICommandSetStaticUniformBuffer)
{
    EShaderType TargetShader;
    FUniformBufferStaticSlot Slot;
    FRHIUniformBuffer* UniformBuffer;
    FRHICommandSetStaticUniformBuffer(EShaderType InTarget, FUniformBufferStaticSlot InSlot, FRHIUniformBuffer* InUB)
        : TargetShader(InTarget), Slot(InSlot), UniformBuffer(InUB) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetDynamicUniformBuffer)
{
    EShaderType TargetShader;
    FUniformBufferStaticSlot Slot;
    FRHIUniformBuffer* UniformBuffer;
    FRHICommandSetDynamicUniformBuffer(EShaderType InTarget, FUniformBufferStaticSlot InSlot, FRHIUniformBuffer* InUB)
        : TargetShader(InTarget), Slot(InSlot), UniformBuffer(InUB) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetShaderResourceView)
{
    EShaderType TargetShader;
    FShaderResourceStaticSlot Slot;
    FRHIView* SRV;
    FRHICommandSetShaderResourceView(EShaderType InTarget, FShaderResourceStaticSlot InSlot, FRHIView* InSRV)
        : TargetShader(InTarget), Slot(InSlot), SRV(InSRV) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetSampler)
{
    EShaderType TargetShader;
    FSamplerStaticSlot Slot;
    FRHISamplerState* SamplerState;
    FRHICommandSetSampler(EShaderType InTarget, FSamplerStaticSlot InSlot, FRHISamplerState* InState)
        : TargetShader(InTarget), Slot(InSlot), SamplerState(InState) {}
    void Execute(FRHICommandListBase& CmdList);
};

// 5. Pipeline / Fixed States
FRHICOMMAND_MACRO(FRHICommandSetRasterizerState)
{
    FRHIRasterizerState* State;
    FRHICommandSetRasterizerState(FRHIRasterizerState* InState) : State(InState) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetBlendState)
{
    FRHIBlendState* State;
    FLinearColor BlendFactor;
    uint32 SampleMask;
    FRHICommandSetBlendState(FRHIBlendState* InState, const FLinearColor& InBlendFactor, uint32 InSampleMask)
        : State(InState), BlendFactor(InBlendFactor), SampleMask(InSampleMask) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetDepthStencilState)
{
    FRHIDepthStencilState* State;
    uint32 StencilRef;
    FRHICommandSetDepthStencilState(FRHIDepthStencilState* InState, uint32 InStencilRef)
        : State(InState), StencilRef(InStencilRef) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandSetViewport)
{
    FRHIViewport* Viewport;
    FRHICommandSetViewport(FRHIViewport* InViewport) : Viewport(InViewport) {}
    void Execute(FRHICommandListBase& CmdList);
};

// 6. Output Merger
FRHICOMMAND_MACRO(FRHICommandSetRenderTargets)
{
    uint32 NumRTVs;
    /**
     * We assume that RTV won't destruct before the command is executed.
     */
    TArray<FRHIView*> RTVs;
    FRHIView* DSV;
    FRHICommandSetRenderTargets(uint32 InNumRTVs, FRHIView* const* InRTVs, FRHIView* InDSV)
        : NumRTVs(InNumRTVs), DSV(InDSV)
    {
        RTVs.Reserve(NumRTVs);
        for (uint32 i = 0; i < NumRTVs; ++i)
        {
            RTVs.Add(InRTVs[i]);
        }
    }
    void Execute(FRHICommandListBase& CmdList);
};

// 7. Updates (Buffer / Uniform / Texture / Viewport)
/**
 * The InData is copied inside, so you can modify the data after the command is created
 */
FRHICOMMAND_MACRO(FRHICommandUpdateBuffer)
{
    FRHIBuffer* Buffer;
    void* Copy;
    uint32 Size;
    FRHICommandUpdateBuffer(FRHIBuffer* InBuffer, const void* InData, uint32 InSize)
        : Buffer(InBuffer), Copy(nullptr), Size(InSize)
    {
        if (InSize)
        {
            Copy = std::malloc(InSize);
            std::memcpy(Copy, InData, InSize);
        }
    }
    ~FRHICommandUpdateBuffer()
    {
        if (Copy) { std::free(Copy); }
    }
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandUpdateUniformBuffer)
{
    FRHIUniformBuffer* UniformBuffer;
    void* Copy;
    uint32 Size;
    FRHICommandUpdateUniformBuffer(FRHIUniformBuffer* InUB, const void* InData, uint32 InSize)
        : UniformBuffer(InUB), Copy(nullptr), Size(InSize)
    {
        if (InSize)
        {
            Copy = std::malloc(InSize);
            std::memcpy(Copy, InData, InSize);
        }
    }
    ~FRHICommandUpdateUniformBuffer()
    {
        if (Copy) { std::free(Copy); }
    }
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandUpdateTexture)
{
    FRHITexture* Texture;
    void* Copy;
    uint32 Size;
    FRHICommandUpdateTexture(FRHITexture* InTex, const void* InData, uint32 InSize)
        : Texture(InTex), Copy(nullptr), Size(InSize)
    {
        if (InSize)
        {
            Copy = std::malloc(InSize);
            std::memcpy(Copy, InData, InSize);
        }
    }
    ~FRHICommandUpdateTexture()
    {
        if (Copy) { std::free(Copy); }
    }
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandUpdateViewport)
{
    FRHIViewport* Viewport;
    FRHIViewportDesc Desc;
    FRHICommandUpdateViewport(FRHIViewport* InViewport, const FRHIViewportDesc& InDesc)
        : Viewport(InViewport), Desc(InDesc) {}
    void Execute(FRHICommandListBase& CmdList);
};

// 8. Clear
FRHICOMMAND_MACRO(FRHICommandClearColorTexture)
{
    FRHITexture* Texture;
    FLinearColor ClearColor;
    FRHICommandClearColorTexture(FRHITexture* InTexture, const FLinearColor& InColor)
        : Texture(InTexture), ClearColor(InColor) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandClearDepthTexture)
{
    FRHITexture* Texture;
    float Depth;
    uint8 Stencil;
    FRHICommandClearDepthTexture(FRHITexture* InTexture, float InDepth, uint8 InStencil)
        : Texture(InTexture), Depth(InDepth), Stencil(InStencil) {}
    void Execute(FRHICommandListBase& CmdList);
};

// 9. Draw
FRHICOMMAND_MACRO(FRHICommandDrawPrimitive)
{
    uint32 BaseVertexIndex;
    uint32 NumVertices;
    uint32 NumInstances;
    FRHICommandDrawPrimitive(uint32 InBaseVertex, uint32 InNumVerts, uint32 InNumInst)
        : BaseVertexIndex(InBaseVertex), NumVertices(InNumVerts), NumInstances(InNumInst) {}
    void Execute(FRHICommandListBase& CmdList);
};

FRHICOMMAND_MACRO(FRHICommandDrawIndexedPrimitive)
{
    uint32 BaseVertexIndex;
    uint32 StartIndex;
    uint32 NumIndices;
    uint32 NumInstances;
    FRHICommandDrawIndexedPrimitive(uint32 InBaseVertex, uint32 InStartIndex, uint32 InNumIndices, uint32 InNumInstances)
        : BaseVertexIndex(InBaseVertex), StartIndex(InStartIndex), NumIndices(InNumIndices), NumInstances(InNumInstances) {}
    void Execute(FRHICommandListBase& CmdList);
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

    virtual ~FRHICommandListBase()
    {
        // 사용자가 Execute 전에 파괴하면 누수 방지 위해 남은 명령 소멸
        if (Root)
        {
            DestroyUnexecuted();
        }
    }

    FRHICommandListBase(FRHICommandListBase const&) = delete;
    FRHICommandListBase& operator=(FRHICommandListBase const&) = delete;
    FRHICommandListBase(FRHICommandListBase&& Other) noexcept
    {
        MoveFrom(Other);
    }
    FRHICommandListBase& operator=(FRHICommandListBase&& Other) noexcept
    {
        if (this != &Other)
        {
            DestroyUnexecuted();
            MoveFrom(Other);
        }
        return *this;
    }

    uint32_t GetNumCommands() const { return NumCommands; }
    bool HasCommands() const { return Root != nullptr; }

    // Called by ENQUEUE_RENDER_COMMAND
    template <typename RHICmdListType, typename LAMBDA>
    FORCEINLINE void EnqueueLambda(LAMBDA&& Fn)
    {
        if (bBypass)
        {
            Fn(static_cast<RHICmdListType&>(*this));
            return;
        }
        using CmdType = TRHILambdaCommand<RHICmdListType, std::decay_t<LAMBDA>>;
        void* Mem = AllocCommand(sizeof(CmdType), alignof(CmdType));
        new (Mem) CmdType(std::forward<LAMBDA>(Fn));
    }

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

    // Execute + Reset allocator
    void ExecuteAndReset()
    {
        Execute();
        Allocator.Reset();
    }

    // 기록만 비우고 allocator 재활용
    FORCEINLINE void ResetAndRelease()
    {
        DestroyUnexecuted();
        Root = nullptr;
        CommandLink = &Root;
        NumCommands = 0;
        Allocator.Reset();
    }
    
    IRHICommandContext& GetContext() const { return *GraphicsContext; }
    
protected:
    FORCEINLINE void* Alloc(size_t AllocSize, size_t Alignment)
    {
        return Allocator.Alloc(AllocSize, Alignment);
    }
    
    FORCEINLINE void* AllocCommand(size_t Size, size_t Alignment)
    {
        FRHICommandBase* Result = static_cast<FRHICommandBase*>(Allocator.Alloc(Size, Alignment));
        
        *CommandLink = Result;
        CommandLink = &Result->Next;
        ++NumCommands;
        return Result;
    }

    template <typename TCmd>
    FORCEINLINE void* AllocCommand()
    {
        return AllocCommand(sizeof(TCmd), alignof(TCmd));
    }

private:
    void MoveFrom(FRHICommandListBase& Other)
    {
        Root = Other.Root;
        CommandLink = Other.CommandLink;
        NumCommands = Other.NumCommands;

        Other.Root = nullptr;
        Other.CommandLink = &Other.Root;
        Other.NumCommands = 0;
    }

    void DestroyUnexecuted()
    {
        FRHICommandBase* Node = Root;
        while (Node)
        {
            FRHICommandBase* Next = Node->Next;
            Node->~FRHICommandBase();
            Node = Next;
        }
    }

protected:
    IRHICommandContext* GraphicsContext = nullptr;
    FRHICommandBase* Root = nullptr;
    FRHICommandBase** CommandLink = nullptr;
    uint32_t           NumCommands = 0;

    FLinearAllocator   Allocator;

public:
    // Creation is defined in FRHICommandListBase, not FRHICommandList
protected:
    // TODO : not using commandlist yet. Change when implementing command list
    inline bool Bypass() const { return RHI_COMMANDLIST_BYPASS; } 

};

class IRHICommandContext;

class FRHICommandList : public FRHICommandListBase
{
    friend class FRHICommandListExecutor;
public:
    virtual ~FRHICommandList() = default;

    // Render Passes
    void BeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* Name)
    {
        if(Bypass())
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
        if(Bypass())
        {
            GetContext().RHIEndRenderPass();
        }
        else
        {
            ALLOC_COMMAND(FRHICommandEndRenderPass)();
        }
    }

    // Input Assembly
    void SetPrimitiveTopology(EPrimitiveType PrimitiveType)
    {
        if(Bypass())
        {
            GetContext().RHISetPrimitiveTopology(PrimitiveType);
        }
        else
        {
            ALLOC_COMMAND(FRHICommandSetPrimitiveTopology)(PrimitiveType);
        }
    }
    void SetInputLayout(FRHIInputLayout* InputLayout)
    {
        if(Bypass())
        {
            GetContext().RHISetInputLayout(InputLayout);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetInputLayout)(InputLayout);
        }
    }
    void SetVertexBuffer(uint32 Slot, FRHIBuffer* VertexBuffer, uint32 Stride, uint32 Offset)
    {
        if(Bypass())
        {
            GetContext().RHISetVertexBuffer(Slot, VertexBuffer, Stride, Offset);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetVertexBuffer)(Slot, VertexBuffer, Stride, Offset);
        }
    }
    void SetIndexBuffer(uint32 Slot, FRHIBuffer* IndexBuffer, uint32 Offset) // Index buffer uses uint32
    {
        if(Bypass())
        {
            GetContext().RHISetIndexBuffer(Slot, IndexBuffer, Offset);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetIndexBuffer)(Slot, IndexBuffer, Offset);
        }
    }

    // Shaders
    void SetVertexShader(FRHIVertexShader* VertexShader)
    {
        if(Bypass())
        {
            GetContext().RHISetVertexShader(VertexShader);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetVertexShader)(VertexShader);
        }
    }
    void SetPixelShader(FRHIPixelShader* PixelShader)
    {
        if(Bypass())
        {
            GetContext().RHISetPixelShader(PixelShader);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetPixelShader)(PixelShader);
        }
    }
    void SetComputeShader(FRHIComputeShader* ComputeShader)
    {
        if(Bypass())
        {
            GetContext().RHISetComputeShader(ComputeShader);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetComputeShader)(ComputeShader);
        }
    }
    void SetGeometryShader(FRHIGeometryShader* GeometryShader)
    {
        if(Bypass())
        {
            GetContext().RHISetGeometryShader(GeometryShader);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetGeometryShader)(GeometryShader);
        }
    }

    void SetStaticUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer)
    {
        if(Bypass())
        {
            GetContext().RHISetStaticUniformBuffer(TargetShader, Slot, UniformBuffer);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetStaticUniformBuffer)(TargetShader, Slot, UniformBuffer);
        }
    }
    void SetDynamicUniformBuffer(EShaderType TargetShader, FUniformBufferStaticSlot Slot, FRHIUniformBuffer* UniformBuffer)
    {
        if(Bypass())
        {
            GetContext().RHISetDynamicUniformBuffer(TargetShader, Slot, UniformBuffer);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetDynamicUniformBuffer)(TargetShader, Slot, UniformBuffer);
        }
    }
    void SetShaderResourceView(EShaderType TargetShader, FShaderResourceStaticSlot Slot, FRHIView* SRV)
    {
        if(Bypass())
        {
            GetContext().RHISetShaderResourceView(TargetShader, Slot, SRV);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetShaderResourceView)(TargetShader, Slot, SRV);
        }
    }
    void SetSampler(EShaderType TargetShader, FSamplerStaticSlot Slot, FRHISamplerState* SamplerState)
    {
        if(Bypass())
        {
            GetContext().RHISetSampler(TargetShader, Slot, SamplerState);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetSampler)(TargetShader, Slot, SamplerState);
        }
    }

    // Rasterizer
    void SetRasterizerState(FRHIRasterizerState* RasterizerState)
    {
        if(Bypass())
        {
            GetContext().RHISetRasterizerState(RasterizerState);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetRasterizerState)(RasterizerState);
        }
    }
    void SetBlendState(FRHIBlendState* BlendState, const FLinearColor& BlendFactor, uint32 SampleMask)
    {
        if(Bypass())
        {
            GetContext().RHISetBlendState(BlendState, BlendFactor, SampleMask);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetBlendState)(BlendState, BlendFactor, SampleMask);
        }
    }
    void SetDepthStencilState(FRHIDepthStencilState* DepthStencilState, uint32 StencilRef)
    {
        if(Bypass())
        {
            GetContext().RHISetDepthStencilState(DepthStencilState, StencilRef);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetDepthStencilState)(DepthStencilState, StencilRef);
        }
    }
    void SetViewport(FRHIViewport* Viewport)
    {
        if(Bypass())
        {
            GetContext().RHISetViewport(Viewport);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetViewport)(Viewport);
        }
    }

    // Output Merger
    void SetRenderTargets(uint32 NumRTVs, FRHIView* const* RTVs, FRHIView* DSV)
    {
        if(Bypass())
        {
            GetContext().RHISetRenderTargets(NumRTVs, RTVs, DSV);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandSetRenderTargets)(NumRTVs, RTVs, DSV);
        }
    }

    // Updates
    void UpdateBuffer(FRHIBuffer* Buffer, const void* Data, uint32 Size)
    {
        if(Bypass())
        {
            GetContext().RHIUpdateBuffer(Buffer, Data, Size);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandUpdateBuffer)(Buffer, Data, Size);
        }
    }
    void UpdateUniformBuffer(FRHIUniformBuffer* UniformBuffer, const void* Data, uint32 Size)
    {
        if(Bypass())
        {
            GetContext().RHIUpdateUniformBuffer(UniformBuffer, Data, Size);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandUpdateUniformBuffer)(UniformBuffer, Data, Size);
        }
    }
    void UpdateTexture(FRHITexture* Texture, const void* Data, uint32 Size)
    {
        if(Bypass())
        {
            GetContext().RHIUpdateTexture(Texture, Data, Size);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandUpdateTexture)(Texture, Data, Size);
        }
    }
    void UpdateViewport(FRHIViewport* Viewport, const FRHIViewportDesc& Desc)
    {
        if(Bypass())
        {
            GetContext().RHIUpdateViewport(Viewport, Desc);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandUpdateViewport)(Viewport, Desc);
        }
    }

    // Clear
    /*
    * Note that BeginRenderPass() clears the render targets if specified in FRHIRenderPassInfo.
    */
    void ClearColorTexture(FRHITexture* Texture, const FLinearColor& ClearColor)
    {
        if(Bypass())
        {
            GetContext().RHIClearColorTexture(Texture, ClearColor);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandClearColorTexture)(Texture, ClearColor);
        }
    }
    void ClearDepthTexture(FRHITexture* Texture, float Depth, uint8 Stencil)
    {
        if(Bypass())
        {
            GetContext().RHIClearDepthTexture(Texture, Depth, Stencil);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandClearDepthTexture)(Texture, Depth, Stencil);
        }
    }

    // Draw
    void DrawPrimitive(uint32 BaseVertexIndex, uint32 NumVertices, uint32 NumInstances)
    {
        if(Bypass())
        {
            GetContext().RHIDrawPrimitive(BaseVertexIndex, NumVertices, NumInstances);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandDrawPrimitive)(BaseVertexIndex, NumVertices, NumInstances);
        }

    }
    void DrawIndexedPrimitive(int32 BaseVertexIndex, uint32 StartIndex, uint32 NumIndices, uint32 NumInstances)
    {
        if(Bypass())
        {
            GetContext().RHIDrawIndexedPrimitive(BaseVertexIndex, StartIndex, NumIndices, NumInstances);
        }
        else
        {
            // ALLOC_COMMAND(FRHICommandDrawIndexedPrimitive)(BaseVertexIndex, StartIndex, NumIndices, NumInstances);
        }
    }
};

class FRHICommandListImmediate : public FRHICommandList
{
    friend class FRHICommandListExecutor;

public:
    virtual ~FRHICommandListImmediate() override = default;

    static inline FRHICommandListImmediate& Get();

    void BeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI);
    void EndDrawingViewport(FRHIViewport* Viewport, bool bPresent);

    // Flush to GPU and waits until all commands are executed
    void ImmediateFlush()
    {
        // GRHICommandList.Submit
    }
};

class FRHICommandListExecutor
{
public:
    static inline FRHICommandListImmediate& GetImmediateCommandList()
    {
        return GRHICommandList.CommandListImmediate;
    }

    void Submit();

    friend class FRHICommandListBase;
    friend class FRHICommandListImmediate;

    // We now only use FRHICommandListImmediate. No other command lists.
    FRHICommandListImmediate CommandListImmediate;
};

extern FRHICommandListExecutor GRHICommandList;

} // namespace RHI

#include "RHICommandListCommandExecutes.inl"
