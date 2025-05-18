#include "SubRenderer.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/UnrealClient.h"

void FSubRenderer::Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, USubEngine* InEngine)
{
    Engine = InEngine;
    Graphics = InGraphics;
    BufferManager = InBufferManager;
}

void FSubRenderer::PrepareRender(FEditorViewportClient* Viewport)
{
    const EViewModeIndex ViewMode = Viewport->GetViewMode();
    TargetViewport = Viewport;
    //UpdateViewCamera(Viewport);
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(EResourceType::ERT_Scene);
    FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);

    D3D11_VIEWPORT vp = Viewport->GetD3DViewport();
    Graphics->DeviceContext->RSSetViewports(1, &vp);

    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    
    Graphics->DeviceContext->ClearRenderTargetView(
        RenderTargetRHI->RTV,
        Graphics->ClearColor
    );

    Graphics->DeviceContext->ClearDepthStencilView(
        Graphics->DeviceDSV,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f, 0
    );

    //Graphics->DeviceContext->OMSetRenderTargets(
    //    1,
    //    &RenderTargetRHI->RTV,
    //    nullptr
    //);

    Graphics->DeviceContext->OMSetRenderTargets(
        1,
        &Graphics->BackBufferRTV,
        Graphics->DeviceDSV
    );

    if (Viewport->GetViewMode() == EViewModeIndex::VMI_Wireframe)
        Graphics->DeviceContext->RSSetState(Graphics->RasterizerWireframeBack);
    else
        Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);

    Graphics->DeviceContext->OMSetDepthStencilState(
        Graphics->DepthStencilState,
        0);

}

void FSubRenderer::Render()
{

}

void FSubRenderer::ClearRender()
{
}

void FSubRenderer::Release()
{
}
