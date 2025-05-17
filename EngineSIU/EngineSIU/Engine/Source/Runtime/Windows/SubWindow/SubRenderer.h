#pragma once

class FGraphicsDevice;
class USubEngine;
class FDXDBufferManager;
class FEditorViewportClient;

class FSubRenderer
{
public:

    void Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, USubEngine* InEngine);
    void PrepareRender(FEditorViewportClient* Viewport);
    void Render();
    void ClearRender();
    void Release();
private:

    FGraphicsDevice* Graphics;
    USubEngine* Engine = nullptr;
    FDXDBufferManager* BufferManager;
    FEditorViewportClient* TargetViewport = nullptr;
};

