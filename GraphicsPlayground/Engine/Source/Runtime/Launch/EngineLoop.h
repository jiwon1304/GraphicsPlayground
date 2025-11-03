#pragma once
#include "Core/HAL/PlatformType.h"
#include <memory>

class FSlateAppMessageHandlerBase;
class UnrealEd;
class UImGuiManager;
class UWorld;
class FEditorViewportClient;
class SSplitterV;
class SSplitterH;
class FGraphicDevice;
class SLevelEditor;
class FDXDBufferManager;
class USubEngine;
class IGPUTimingManager;
class FEngineProfiler;
class FResourceManager;
class FGraphicsDevice;
class FRenderer;
class UPrimitiveDrawBatch;
class FEngineLoop
{
public:
    FEngineLoop();

    int32 PreInit();
    int32 Init(HINSTANCE hInstance);
    void Render() const;
    void Tick();
    void Exit();

    void GetClientSize(uint32 &OutWidth, uint32 &OutHeight) const;

    void OpenParticleSystemViewer();
    void SubEngineControl();

private:
    void WindowInit(HINSTANCE hInstance);
    static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    void UpdateUI();

public:
    FGraphicsDevice* GraphicDevice = nullptr;
    FRenderer* Renderer = nullptr;
    static UPrimitiveDrawBatch* PrimitiveDrawBatch;
    FResourceManager* ResourceManager = nullptr;
    static uint32 TotalAllocationBytes;
    static uint32 TotalAllocationCount;

    HWND AppWnd;

    IGPUTimingManager* GPUTimingManager = nullptr;
    FEngineProfiler* EngineProfiler = nullptr;

    FGraphicsDevice* ParticleViewerGD = nullptr;
    HWND ParticleViewerWnd;
    void ParticleSubWindowInit(HINSTANCE hInstance);
    void CleanupSubWindow();

    USubEngine* ParticleSubEngine = nullptr;

private:
    UImGuiManager* UIManager;
    struct ImGuiContext *CurrentImGuiContext;
    // TODO: GWorld 제거, Editor들 EditorEngine으로 넣기

    FSlateAppMessageHandlerBase* AppMessageHandler = nullptr;
    SLevelEditor* LevelEditor = nullptr;
    UnrealEd* UnrealEditor = nullptr;
    FDXDBufferManager* BufferManager = nullptr; // TODO: UEngine으로 옮겨야함.

    bool bIsExit = false;
    // @todo Option으로 선택 가능하도록
    int32 TargetFPS = 999;

public:
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; }

    FSlateAppMessageHandlerBase *GetAppMessageHandler() const { return AppMessageHandler; }
};

extern FEngineLoop GEngineLoop;