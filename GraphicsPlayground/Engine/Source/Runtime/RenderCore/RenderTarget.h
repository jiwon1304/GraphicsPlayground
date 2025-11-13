#pragma once

#include "RenderResource.h"
#include "RHI/PixelFormat.h"
#include "RHI/RHIFwd.h"

struct FRenderTarget;

struct FRenderTargetDesc
{
    FRenderTargetDesc(
        float InClearColor[4],
        uint16 InWidth,
        uint16 InHeight,
        EPixelFormat InFormat)
        : Width(InWidth)
        , Height(InHeight)
        , Format(InFormat)
    {
        FPlatformMemory::Memcpy(ClearColor, InClearColor, sizeof(float) * 4);
    }

    float ClearColor[4];
    uint16 Width;
    uint16 Height;
    EPixelFormat Format;
};

/**
 * Attach a render target to a texture
 */
struct FRenderTarget : public FRenderResource
{
    FRenderTarget(FRHITexture* Texture, const FRenderTargetDesc& InDesc)
        : Texture(Texture)
        , Desc(InDesc)
    {
    }

    virtual ~FRenderTarget() {}

    virtual void Release() {}
    
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

    virtual void ReleaseRHI() override;

private:
    FTextureRHIRef Texture;
    FRenderTargetDesc Desc;
};