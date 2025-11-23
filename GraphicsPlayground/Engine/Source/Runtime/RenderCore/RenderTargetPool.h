#pragma once

#include "Container/Map.h"
#include "RHI/RHIResources.h"

struct FRenderTarget;
struct FRenderTargetDesc;

enum class EResourceType : uint8
{
    ERT_Compositing,
    ERT_Scene,
    ERT_PP_Fog,
    ERT_PP_ShapeOverlay,
    ERT_PP_CameraEffect,
    ERT_PP_Blur, // blur는 여러개 쓸수있으니까...
    ERT_Debug,
    ERT_Editor,
    ERT_Gizmo,
    ERT_Overlay,
    ERT_PostProcessCompositing,
    ERT_MAX,
};

/**
 * Different from Unreal Engine's render target pool.
 * This is a simple list, not a pool that recycles render targets.
 * TODO : Implement a proper pool if needed.
 */
class FRenderTargetPool
{
    // Create render targets for scene, gizmo, compositing
    void Initialize(const FRHITextureCreateDesc& InDesc);
    
    void Release();

    FRenderTarget* GetRenderTarget(EResourceType Type);
private:
    TMap<EResourceType, FRenderTarget*> RenderTargets;

    FRHITextureCreateDesc Desc;

    /**
     * Create new texture that can be used as render target and SRV.
     * Note that there is no way to change non-RTV texture into RTV texture later.
     */
    static FRenderTarget* CreateRenderTarget(FRHICommandListBase& RHICmdList, const FRHITextureCreateDesc& InDesc);
};

extern FRenderTargetPool GRenderTargetPool;
