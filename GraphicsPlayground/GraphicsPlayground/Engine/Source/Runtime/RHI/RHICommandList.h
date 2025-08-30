#pragma once

#include "RHI/RHIFwd.h"
#include "Math/Color.h"
#include "RHI/RHIDefinitions.h"
#include "Container/Array.h"
#include <HAL/LinearAllocator.h>

namespace RHI
{
/*
* RHICommand contains a lamdba function, where the size varies.
* To avoid heap allocation for each command, we use a linear allocator to allocate memory for commands.
*/
struct FRHICommandBase
{
    FRHICommandBase* Next = nullptr;
    virtual void ExecuteAndDestruct(FRHICommandListBase& CmdList) = 0;
    virtual ~FRHICommandBase() = default;
};

template <typename RHICmdListType, typename LAMBDA>
struct TRHILambdaCommand final : public FRHICommandBase
{
    LAMBDA Lambda;

    explicit TRHILambdaCommand(LAMBDA&& InLambda)
        : Lambda(Forward<LAMBDA>(InLambda))
    {
    }

    void ExecuteAndDestruct(FRHICommandListBase& CmdList) override final
    {
        Lambda(static_cast<RHICmdListType&>(CmdList));
        this->~TRHILambdaCommand();
    }
};

class FRHICommandListBase
{
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

    // Record API -------------------------------------------------
    template <typename RHICmdListType, typename LAMBDA>
    void EnqueueLambda(LAMBDA&& Fn)
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
    // 한 번 재생; 재생 후에는 Root/CommandLink를 초기화 (ResetAndRelease 호출 권장)
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
    void ResetAndRelease()
    {
        DestroyUnexecuted();
        Root = nullptr;
        CommandLink = &Root;
        NumCommands = 0;
        Allocator.Reset();
    }

protected:
    void* AllocCommand(size_t Size, size_t Alignment)
    {
        FRHICommandBase* Result = static_cast<FRHICommandBase*>(Allocator.Alloc(Size, Alignment));

        *CommandLink = Result;
        CommandLink = &Result->Next;
        ++NumCommands;
        return Result;
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
    FRHICommandBase* Root = nullptr;
    FRHICommandBase** CommandLink = nullptr;
    uint32_t           NumCommands = 0;

    FLinearAllocator   Allocator;
};

/*
* 실제론 Immediate Context를 이용하기 때문에 commandlist를 사용하진 않지만, 향후 Deferred Context를 사용할 경우를 대비해 인터페이스는 유지.
*/
class FRHICommandList : FRHICommandListBase
{
public:
    virtual ~FRHICommandList() = default;
    virtual void Init() = 0;
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

class FRHICommandListImmediate : public FRHICommandList
{
public:
    virtual ~FRHICommandListImmediate() override = default;

    virtual void ImmediateFlush();
};
} // namespace RHI
