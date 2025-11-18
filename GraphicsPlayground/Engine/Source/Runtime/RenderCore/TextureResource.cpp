#include "TextureResource.h"

void FTextureRenderTarget2DResource::InitRHI(FRHICommandListBase &RHICmdList)
{
    if (TargetSizeX == 0 || TargetSizeY == 0)
    {
        return;
    }

}

void FTextureRenderTarget2DResource::ReleaseRHI()
{

}

void FTexture2DResource::InitRHI(FRHICommandListBase &RHICmdList)
{
    const FRHITextureCreateDesc Desc = 
        FRHITextureCreateDesc::Create2D(
            TEXT("FTexture2DResource"),
            Owner->GetSizeX(),
            Owner->GetSizeY(),
            Owner->GetPixelFormat()
        )
        .SetNumMips(Owner->GetNumMips())
        .SetArraySize(1)
        .SetDebugName(*TextureName.ToString())
        .SetNumSamples(1);
    
    TextureRHI = RHICreateTexture(RHICmdList, Desc, nullptr);
}
