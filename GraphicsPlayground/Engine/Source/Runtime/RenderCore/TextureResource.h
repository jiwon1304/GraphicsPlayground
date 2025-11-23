#pragma once

#include "RenderResource.h"
#include "RHI/RHIResources.h"
#include "Engine/UnrealClient.h"

class FTexture2DResource;
class FTexture3DResource;
class FTexture2DArrayResource;
class FTextureCubeMapResource;

class UTexture;
class UTexture2D;

struct FTextureCreateDesc
{
    uint32 SizeX = 0;
    uint32 SizeY = 0;
    uint32 SizeZ = 0;

    EPixelFormat Format = PF_Unknown;
    uint16 ArraySize = 1;
    uint8 NumMips = 1;
};

/**
 * 
 */
class FTextureResource : public FTexture
{
public:
    FTextureResource();
    virtual ~FTextureResource();

    // For casting
    virtual FTexture2DResource* GetTexture2DResource() { return nullptr; }
    virtual FTexture3DResource* GetTexture3DResource() { return nullptr; }
    virtual FTexture2DArrayResource* GetTexture2DArrayResource() { return nullptr; }
    virtual FTextureCubeMapResource* GetTextureCubeMapResource() { return nullptr; }

    virtual const FTexture2DResource* GetTexture2DResource() const { return nullptr; }
    virtual const FTexture3DResource* GetTexture3DResource() const { return nullptr; }
    virtual const FTexture2DArrayResource* GetTexture2DArrayResource() const { return nullptr; }
    virtual const FTextureCubeMapResource* GetTextureCubeMapResource() const { return nullptr; }

    FTextureRHIRef GetTextureRHIRef()
    {
        return TextureRHI;
    }

protected:
    FTextureRHIRef TextureRHI;
};

/**
 * 
 */
class FTexture2DResource : public FTextureResource
{
public:
    FTexture2DResource(UTexture2D* InOwner, FTextureCreateDesc InCreateDesc);
    ~FTexture2DResource();

    virtual FTexture2DResource* GetTexture2DResource() override { return this; }
    virtual const FTexture2DResource* GetTexture2DResource() const override { return this; }

    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
    virtual void ReleaseRHI() override;

protected:
    FName TextureName;

    FTextureCreateDesc State;

    UTexture2D* Owner;
};

class FTextureRenderTargetResource : public FTextureResource, public FRenderTarget
{
    FTextureRenderTargetResource() {}

    virtual uint32 GetSizeX() const = 0;
    virtual uint32 GetSizeY() const = 0;

	virtual class FTextureRenderTarget2DResource* GetTextureRenderTarget2DResource() { return nullptr; }
};

class FTextureRenderTarget2DResource : public FTextureRenderTargetResource
{
public:
    FTextureRenderTarget2DResource() = default;
    virtual ~FTextureRenderTarget2DResource() = default;

    virtual FTextureRenderTarget2DResource* GetTextureRenderTarget2DResource() override { return this; }

    virtual uint32 GetSizeX() const override { return TargetSizeX; }
    virtual uint32 GetSizeY() const override { return TargetSizeY; }
    EPixelFormat GetFormat() const { return Format; }

    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
    virtual void ReleaseRHI() override;
    
protected:
    friend class UTextureRenderTarget2D;
    void Resize(uint32 NewSizeX, uint32 NewSizeY, uint32 NewNumMips);

private:
    const class UTextureRenderTarget2D* Owner;

    FLinearColor ClearColor;
    EPixelFormat Format;
    uint32 TargetSizeX, TargetSizeY;
    uint32 TargetNumMips;
};
