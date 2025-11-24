#include "RenderResource.h"
#include "RHI/DynamicRHI.h"
#include "RHI/RHIResources.h"

FTexture::FTexture()
{
}

FTexture::~FTexture()
{
}

uint32 FTexture::GetSizeX() const
{
    return TextureRHI->GetDesc().DimX;
}

uint32 FTexture::GetSizeY() const
{
    return TextureRHI->GetDesc().DimY;
}

uint32 FTexture::GetSizeZ() const
{
    return TextureRHI->GetDesc().DimZ;
}

void FTexture::ReleaseRHI()
{
    TextureRHI.SafeRelease();
    SamplerStateRHI.SafeRelease();
}

FVertexBuffer::~FVertexBuffer()
{
}

void FVertexBuffer::ReleaseRHI(){
    VertexBufferRHI->Release();
}

FTextureWithSRV::FTextureWithSRV()
{
}

FTextureWithSRV::~FTextureWithSRV()
{
}

void FTextureWithSRV::ReleaseRHI()
{
    ShaderResourceViewRHI.SafeRelease();
}

FIndexBuffer::~FIndexBuffer()
{
}

void FIndexBuffer::ReleaseRHI()
{
}

FRenderResource::FRenderResource()
{
}

FRenderResource::~FRenderResource()
{
}

void FRenderResource::InitResource(FRHICommandListBase &RHICmdList)
{
}

void FRenderResource::ReleaseResource()
{
}
