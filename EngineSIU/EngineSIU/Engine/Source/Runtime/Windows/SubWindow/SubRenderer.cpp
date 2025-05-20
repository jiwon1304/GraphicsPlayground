#include "SubRenderer.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/UnrealClient.h"
#include "Renderer/ParticleRenderPass.h"
#include "SubWindow/ParticleSubEngine.h"
void FSubRenderer::Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, USubEngine* InEngine)
{
    Engine = InEngine;
    Graphics = InGraphics;
    BufferManager = InBufferManager;
    ParticleRenderPass = new FParticleRenderPass();
    ParticleRenderPass->Initialize(BufferManager, Graphics, FEngineLoop::Renderer.ShaderManager);
}

void FSubRenderer::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    //ParticleRenderPass->PrepareRenderArr();
    ParticleRenderPass->AddParticleComponent(((UParticleSubEngine*)Engine)->GetParticleSystemComponent());
    const EViewModeIndex ViewMode = Viewport->GetViewMode();
    //TargetViewport = Viewport;
    //UpdateViewCamera(Viewport);
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    //FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(EResourceType::ERT_Scene);
    //FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);

    D3D11_VIEWPORT vp = Viewport->GetD3DViewport();
    Graphics->DeviceContext->RSSetViewports(1, &vp);
    
    //Graphics->DeviceContext->ClearRenderTargetView(
    //    RenderTargetRHI->RTV,
    //    Graphics->ClearColor
    //);

    //Graphics->DeviceContext->ClearDepthStencilView(
    //    Graphics->DeviceDSV,
    //    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
    //    1.0f, 0
    //);

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

    //if (Viewport->GetViewMode() == EViewModeIndex::VMI_Wireframe)
    //    Graphics->DeviceContext->RSSetState(Graphics->RasterizerWireframeBack);
    //else
    //    Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);

    Graphics->DeviceContext->OMSetDepthStencilState(
        Graphics->DepthStencilState,
        0);

}

void FSubRenderer::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    ParticleRenderPass->Render(Viewport);
}

void FSubRenderer::ClearRender()
{
    ParticleRenderPass->ClearRenderArr();
}

void FSubRenderer::Release()
{
    if (ParticleRenderPass)
    {
        delete ParticleRenderPass;
        ParticleRenderPass = nullptr;
    }
}
