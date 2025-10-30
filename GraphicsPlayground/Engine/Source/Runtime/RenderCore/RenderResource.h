#pragma once

#include "RHI/RHIFwd.h"
#include "Core/Misc/CoreGlobals.h"

class FRenderResource
{
public:
    FRenderResource();

    virtual ~FRenderResource();
    
    virtual void InitResource(FRHICommandListBase& RHICmdList);
    
    virtual void ReleaseResource();
    
    /**
     * Update when resource is changed.
     */
    void UpdateRHI(FRHICommandListBase& RHICmdList);
    
protected:    
    virtual void InitRHI(FRHICommandListBase& RHICmdList) {}

    virtual void ReleaseRHI() {}
};

/**
 * for game thread -> render thread 
 */
extern void BeginInitResource(FRenderResource* Resource);

extern void BeginReleaseResource(FRenderResource* Resource);

class FTexture : public FRenderResource
{
protected:
    FTextureRHIRef TextureRHI;
    FSamplerStateRHIRef SamplerStateRHI;
    
public:
    FTexture();
    virtual ~FTexture();

    const FTextureRHIRef& GetTextureRHI() const { return TextureRHI; }

    uint32 GetSizeX() const;
    uint32 GetSizeY() const;
    uint32 GetSizeZ() const;

protected:
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

    // FRenderResource interface
    virtual void ReleaseRHI() override;
};

class FTextureWithSRV : public FTexture
{
    FTextureWithSRV();
    virtual ~FTextureWithSRV();
    
protected:
    virtual void ReleaseRHI() override;
    
    FShaderResourceViewRHIRef ShaderResourceViewRHI;
};

class FVertexBuffer : public FRenderResource
{
    FVertexBuffer();
    virtual ~FVertexBuffer();
    
    void SetRHI(FBufferRHIRef InBufferRHI) { VertexBufferRHI = InBufferRHI; }
    
    const FBufferRHIRef& GetRHI() const { return VertexBufferRHI; }
    
protected:
    virtual void ReleaseRHI() override;

    FBufferRHIRef VertexBufferRHI;
};

class FIndexBuffer : public FRenderResource
{
    FIndexBuffer();
    virtual ~FIndexBuffer();
    
    void SetRHI(FBufferRHIRef InBufferRHI) { IndexBufferRHI = InBufferRHI; }

    const FBufferRHIRef& GetRHI() const { return IndexBufferRHI; }
    
protected:
    virtual void ReleaseRHI() override;

    FBufferRHIRef IndexBufferRHI;
};

