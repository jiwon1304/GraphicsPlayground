#include "RenderTargetPool.h"
#include "RenderTarget.h"
#include "RHI/RHICommandList.h"

FRenderTargetPool GRenderTargetPool;

static float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

void FRenderTargetPool::Initialize(const FRHITextureCreateDesc &InDesc)
{
    assert(InDesc.InitialState & (ERHIAccess::SRVMask | ERHIAccess::RTV));
    Desc = InDesc;
}

void FRenderTargetPool::Release()
{
    for (auto& Pair : RenderTargets)
    {
        if (Pair.Value)
        {
            Pair.Value->Release();
            FPlatformMemory::Free<EAT_Renderer>(Pair.Value, sizeof(FRenderTarget));
        }
    }
    RenderTargets.Empty();
}

FRenderTarget* FRenderTargetPool::GetRenderTarget(EResourceType Type)
{
    if (RenderTargets.Contains(Type))
    {
        return RenderTargets[Type];
    }

    FRenderTarget* NewRenderTarget = CreateRenderTarget(FRHICommandListImmediate::Get(), Desc);
    RenderTargets.Add(Type, NewRenderTarget);
    return NewRenderTarget;
}

// Engine/Source/Runtime/RenderCore/Private/RenderTargetPool.cpp
// FRenderTargetPool::CreateRenderTarget()
FRenderTarget* FRenderTargetPool::CreateRenderTarget(FRHICommandListBase& RHICmdList, const FRHITextureCreateDesc& InDesc)
{
    const ERHIAccess AccessInitial = ERHIAccess::SRVMask | ERHIAccess::RTV;

    FRHITexture* Texture = RHICreateTexture(RHICmdList, InDesc, nullptr);

    FRenderTargetDesc Translated(ClearColor, InDesc.DimX, InDesc.DimY, InDesc.Format);
    FRenderTarget* RenderTarget = new(FPlatformMemory::Malloc<EAT_Renderer>(sizeof(FRenderTarget))) FRenderTarget(Texture, Translated);

    return RenderTarget;
}
