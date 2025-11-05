#pragma once
#include "Core/HAL/PlatformType.h"
#include <memory>

class FGenericSlateAppMessageHandler;
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
class FGenericApplication;
class FGenericWindow;
struct FGenericApplicationInitParams;

class FEngineLoop
{
public:
    FEngineLoop();

    int32 PreInit();
    int32 Init(FGenericApplicationInitParams* MainWindowInstanceHandler);
    void Render() const;
    void Tick();
    void Exit();

    void OpenParticleSystemViewer();
    void SubEngineControl();

private:
    // void WindowInit(HINSTANCE hInstance);
    // static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    void InitApplication(FGenericApplicationInitParams* InAppInitParams);

    void UpdateUI();

public:
    FGraphicsDevice* GraphicDevice = nullptr;
    FRenderer* Renderer = nullptr;
    // static UPrimitiveDrawBatch PrimitiveDrawBatch;
    FResourceManager* ResourceManager = nullptr;
    static uint32 TotalAllocationBytes;
    static uint32 TotalAllocationCount;

    
    IGPUTimingManager* GPUTimingManager = nullptr;
    FEngineProfiler* EngineProfiler = nullptr;
    
    // HWND AppWnd;
    // FGraphicsDevice* ParticleViewerGD = nullptr;
    // HWND ParticleViewerWnd;
    // void ParticleSubWindowInit(HINSTANCE hInstance);
    // void CleanupSubWindow();

public:
    FGenericApplication* Application = nullptr;
    std::shared_ptr<FGenericWindow> MainWindow = nullptr;
    std::shared_ptr<FGenericWindow> ParticleViewerWindow = nullptr;

    // USubEngine* ParticleSubEngine = nullptr;

private:
    UImGuiManager* UIManager = nullptr;
    struct ImGuiContext *CurrentImGuiContext = nullptr;
    // TODO: GWorld 제거, Editor들 EditorEngine으로 넣기

    FGenericSlateAppMessageHandler* AppMessageHandler = nullptr;
    SLevelEditor* LevelEditor = nullptr;
    UnrealEd* UnrealEditor = nullptr;
    FDXDBufferManager* BufferManager = nullptr; // TODO: UEngine으로 옮겨야함.

    bool bIsExit = false;
    // @todo Option으로 선택 가능하도록
    float TargetFPS = 60.0f;

public:
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; }

    FGenericSlateAppMessageHandler *GetAppMessageHandler() const { return AppMessageHandler; }
};

extern FEngineLoop GEngineLoop;