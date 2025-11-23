#pragma once

#include "RHI/RHIResources.h"
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

class FResourceManager;
/**
 * Base class for textures.
 * Derived classes:
 * - FTextureResource
 * - FTextureWithSRV
 * - FStaticShadowDepthMap (@todo)
 * - FSkyTextureCubeResource (@todo)
 */
class FTexture : public FRenderResource
{
    friend class FResourceManager; // @todo  지우기
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
    // FRenderResource interface
    virtual void ReleaseRHI() override;
};

/**
 * Texture which only can be accessed as SRV
 */
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

