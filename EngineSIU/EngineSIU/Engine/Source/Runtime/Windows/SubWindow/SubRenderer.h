#pragma once
#include <memory>
class FGraphicsDevice;
class USubEngine;
class FDXDBufferManager;
class FEditorViewportClient;
class FParticleRenderPass;
class FSubRenderer
{
public:

    void Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, USubEngine* InEngine);
    void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void Render(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void ClearRender();
    void Release();
private:
    void UpdateViewCamera(const std::shared_ptr<FEditorViewportClient>& Viewport);
    FGraphicsDevice* Graphics;
    USubEngine* Engine = nullptr;
    FDXDBufferManager* BufferManager;
    FEditorViewportClient* TargetViewport = nullptr;
    FParticleRenderPass* ParticleRenderPass = nullptr;
};

