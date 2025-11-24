#include "Launch/EngineLoop.h"
#include "Launch/ImGuiManager.h"
#include "Engine/UnrealClient.h"
#include "Classes/Engine/EditorEngine.h"
#include "Editor/LevelEditor/SLevelEditor.h"
#include "Editor/PropertyEditor/ViewportTypePanel.h"
#include "Slate/Widgets/Layout/SSplitter.h"
#include "Editor/UnrealEd/EditorViewportClient.h"
#include "Editor/UnrealEd/UnrealEd.h"
#include "World/World.h"
#include "Renderer/Renderer.h"
#include "Classes/Engine/ResourceMgr.h"

#include "Classes/Engine/EditorEngine.h"
// #include "Renderer/DepthPrePass.h"
// #include "Windows/SubWindow/ParticleSubEngine.h"
// #include "Windows/SubWindow/ImGuiSubWindow.h"
#include "SoundManager.h"
#include "Stats/GPUTimingManager.h"
#include "Stats/ProfilerStatsManager.h"
// #include "Windows/D3D11RHI/DXDBufferManager.h"

// #include "Renderer/TileLightCullingPass.h"
#include "ApplicationCore/Windows/WindowsSlateAppMessageHandler.h"

// 여기부터 새로추가한거
#include "EngineLoop.h"

#include "RenderCore/RenderingThread.h"
#include "RHI/RHICommandList.h"
#include "ApplicationCore/Generic/GenericSlateAppMessageHandler.h"
#include "ApplicationCore/Generic/GenericApplication.h"
#include "ApplicationCore/Generic/GenericWindow.h"
#include "RHI/DynamicRHI.h"
#include "RHI/RHIFwd.h"
#include "Core/HAL/PlatformTime.h"


// Plaform-specific
#ifdef BUILD_PLATFORM_WINDOWS

#include "ApplicationCore/Windows/WindowsApplication.h"
#include "ApplicationCore/Windows/WindowsWindow.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#elif defined(BUILD_PLATFORM_MACOS)

#include "ApplicationCore/Mac/MacApplication.h"
#include "ApplicationCore/Mac/MacWindow.h"
#include "OpenGLDrv/OpenGLDrv.h"
#include "OpenGLDrv/Platform/Mac/MacOpenGLPlatform.h"

#if USE_OPENGL
#include "Runtime/OpenGLDrv/Platform/Mac/MacOpenGLPlatform.h"
#endif // USE_OPENGL

#else
static_assert(false, "Unsupported platform for FEngineLoop");
#endif

FEngineLoop GEngineLoop;
uint32 FEngineLoop::TotalAllocationBytes = 0;
uint32 FEngineLoop::TotalAllocationCount = 0;

FEngineLoop::FEngineLoop() {}

int32 FEngineLoop::PreInit()
{
    return 0;
}

int32 FEngineLoop::Init(FGenericApplicationInitParams* InAppInitParams)
{
    FPlatformTime::InitTiming();

    /**
     * Inits Rendering and RHI stuff
     */
    InitRenderingThread();

    InitApplication(InAppInitParams);

    GPUTimingManager = IGPUTimingManager::CreateGPUTimingManager(FGPUTimingInitParams{ 3 });

    // /* must be initialized before window. */
    // WindowInit(hInstance);

    // GraphicDevice = new FGraphicsDevice;
    // Renderer = new FRenderer;
    // ResourceManager = new FResourceManager;

    // {
    //     GPUTimingManager = CreateGPUTimingManager(FGPUTimingInitParams{ 3, GraphicDevice->Device, GraphicDevice->DeviceContext });
    //     if (!GPUTimingManager)
    //     {
    //         UE_LOG(ELogLevel::Error, TEXT("Failed to initialize GPU Timing Manager!"));
    //     }
    // }
    // EngineProfiler = new FEngineProfiler;
    // ParticleSubEngine = new USubEngine;

    // UIManager = new FImGuiManager;
    // AppMessageHandler = new FGenericSlateAppMessageHandler;
    // LevelEditor = new SLevelEditor;
    // UnrealEditor = new UnrealEd;
    // BufferManager = new FDXDBufferManager;

    // UnrealEditor->Initialize();
    // GraphicDevice->Initialize(AppWnd);

    // EngineProfiler->SetGPUTimingManager(GPUTimingManager);

    // // @todo Table에 Tree 구조로 넣을 수 있도록 수정
    // EngineProfiler->RegisterStatScope(TEXT("Renderer_Render"), FName(TEXT("Renderer_Render_CPU")), FName(TEXT("Renderer_Render_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- DepthPrePass"), FName(TEXT("DepthPrePass_CPU")), FName(TEXT("DepthPrePass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- TileLightCulling"), FName(TEXT("TileLightCulling_CPU")), FName(TEXT("TileLightCulling_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- ShadowPass"), FName(TEXT("ShadowPass_CPU")), FName(TEXT("ShadowPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- StaticMeshPass"), FName(TEXT("StaticMeshPass_CPU")), FName(TEXT("StaticMeshPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- WorldBillboardPass"), FName(TEXT("WorldBillboardPass_CPU")), FName(TEXT("WorldBillboardPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- UpdateLightBufferPass"), FName(TEXT("UpdateLightBufferPass_CPU")), FName(TEXT("UpdateLightBufferPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- FogPass"), FName(TEXT("FogPass_CPU")), FName(TEXT("FogPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- PostProcessCompositing"), FName(TEXT("PostProcessCompositing_CPU")), FName(TEXT("PostProcessCompositing_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- EditorBillboardPass"), FName(TEXT("EditorBillboardPass_CPU")), FName(TEXT("EditorBillboardPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- EditorRenderPass"), FName(TEXT("EditorRenderPass_CPU")), FName(TEXT("EditorRenderPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- LinePass"), FName(TEXT("LinePass_CPU")), FName(TEXT("LinePass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- GizmoPass"), FName(TEXT("GizmoPass_CPU")), FName(TEXT("GizmoPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- CompositingPass"), FName(TEXT("CompositingPass_CPU")), FName(TEXT("CompositingPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- SkinningPass"), FName(TEXT("SkinningPass_CPU")), FName(TEXT("SkinningPass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("|- ParticlePass"), FName(TEXT("ParticlePass_CPU")), FName(TEXT("ParticlePass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("SlatePass"), FName(TEXT("SlatePass_CPU")), FName(TEXT("SlatePass_GPU")));
    // EngineProfiler->RegisterStatScope(TEXT("Physics"), FName(TEXT("PhysicsSceneUpdate")), FName(TEXT("")));

    // BufferManager->Initialize(GraphicDevice->Device, GraphicDevice->DeviceContext);
    // Renderer->Initialize(GraphicDevice, BufferManager, GPUTimingManager);
    // // PrimitiveDrawBatch.Initialize(&GraphicDevice);
    // UIManager->Initialize(AppWnd, GraphicDevice);
    // ResourceManager->Initialize(Renderer, GraphicDevice);

    // uint32 ClientWidth = 0;
    // uint32 ClientHeight = 0;
    // GetClientSize(ClientWidth, ClientHeight);
    // LevelEditor->Initialize(ClientWidth, ClientHeight);

    // GEngine = FObjectFactory::ConstructObject<UEditorEngine>(nullptr);
    // GEngine->Init();

    // ParticleSubWindowInit(hInstance);
    // if (ParticleViewerGD && ParticleViewerWnd)
    // {
    //     ParticleViewerGD->Initialize(ParticleViewerWnd, GraphicDevice->Device);
    //     ParticleViewerGD->ClearColor[0] = 0.03f;
    //     ParticleViewerGD->ClearColor[1] = 0.03f;
    //     ParticleViewerGD->ClearColor[2] = 0.03f;
    // }

    // ParticleSubEngine = FObjectFactory::ConstructObject<UParticleSubEngine>(nullptr);
    // ParticleSubEngine->Initialize(ParticleViewerWnd, ParticleViewerGD, BufferManager, UIManager, UnrealEditor);

    // FSoundManager::GetInstance().Initialize();
    // FSoundManager::GetInstance().LoadSound("fishdream", "Contents/Sounds/fishdream.mp3");
    // FSoundManager::GetInstance().LoadSound("sizzle", "Contents/Sounds/sizzle.mp3");
    // //FSoundManager::GetInstance().PlaySound("fishdream");

    // UpdateUI();

    return 0;
}

void FEngineLoop::Render() const
{
    if (LevelEditor->IsMultiViewport())
    {
        const std::shared_ptr<FEditorViewportClient> ActiveViewportCache = GetLevelEditor()->GetActiveViewportClient();
        for (int Idx = 0; Idx < 4; ++Idx)
        {
            LevelEditor->SetActiveViewportClient(Idx);
            Renderer->Render(LevelEditor->GetActiveViewportClient());
        }

        for (int Idx = 0; Idx < 4; ++Idx)
        {
            LevelEditor->SetActiveViewportClient(Idx);
            Renderer->RenderViewport(LevelEditor->GetActiveViewportClient());
        }
        GetLevelEditor()->SetActiveViewportClient(ActiveViewportCache);
    }
    else
    {
        Renderer->Render(LevelEditor->GetActiveViewportClient());

        Renderer->RenderViewport(LevelEditor->GetActiveViewportClient());
    }
}

void FEngineLoop::Tick()
{
    const double SecondsPerCycle = FPlatformTime::GetSecondsPerCycle();
    const double TargetFrameSeconds = (TargetFPS > 0.0f) ? (1.0 / TargetFPS) : 0.0;
    const uint64 TargetFrameCycles = TargetFrameSeconds > 0.0
        ? static_cast<uint64>(TargetFrameSeconds / SecondsPerCycle)
        : 0;

    uint64 PreviousFrameStartCycles = FPlatformTime::Cycles64();

    while (Application->IsExitRequested() == false)
    {
        FProfilerStatsManager::BeginFrame(); // Clear previous frame stats
        if (GPUTimingManager && GPUTimingManager->IsInitialized())
        {
            GPUTimingManager->BeginFrame(); // Start GPU frame timing
        }
        
        Application->PumpMessages();

        // Deltatime
        const uint64 FrameStartCycles = FPlatformTime::Cycles64();
        const uint64 ElapsedCycles = FrameStartCycles - PreviousFrameStartCycles;
        PreviousFrameStartCycles = FrameStartCycles;
        const float DeltaTime = static_cast<float>(ElapsedCycles * SecondsPerCycle);

        GEngine->Tick(DeltaTime);
        LevelEditor->Tick(DeltaTime);

        Render();

        UIManager->BeginFrame();
        UnrealEditor->Render();

        FConsole::GetInstance().Draw();
        EngineProfiler->Render();

        UIManager->EndFrame();

        // if (ParticleSubEngine->bIsShowing)
        //     ParticleSubEngine->Tick(DeltaTime);

        if (CurrentImGuiContext != nullptr)
        {
            ImGui::SetCurrentContext(CurrentImGuiContext);
        }

        // Pending 처리된 오브젝트 제거
        GUObjectArray.ProcessPendingDestroyObjects();

        if (GPUTimingManager && GPUTimingManager->IsInitialized())
        {
            GPUTimingManager->EndFrame(); // End GPU frame timing
        }

        // SubEngineControl();

        GRHICommandList.Submit();

        // Update ElapsedCycles for deltatime in the next frame
        uint64 FrameDurationCycles = FPlatformTime::Cycles64() - FrameStartCycles;

        if (bFPSLimitEnabled && TargetFrameCycles > 0)
        {
            if (FrameDurationCycles < TargetFrameCycles)
            {
                const uint64 RemainingCycles = TargetFrameCycles - FrameDurationCycles;
                const uint64 SleepMicroseconds = static_cast<uint64>(RemainingCycles * SecondsPerCycle * 1'000'000.0);

                if (SleepMicroseconds > 0)
                {
                    Application->SleepFor(SleepMicroseconds);
                }

                // Re-measure after sleeping to account for timer granularity
                FrameDurationCycles = FPlatformTime::Cycles64() - FrameStartCycles;

                // Busy-wait/yield until we hit the target duration
                while (FrameDurationCycles < TargetFrameCycles)
                {
                    Application->YieldThisThread();
                    FrameDurationCycles = FPlatformTime::Cycles64() - FrameStartCycles;
                }
            }
        }
    }
    FSoundManager::GetInstance().Update();
}

// void FEngineLoop::OpenParticleSystemViewer()
// {
//     if (ParticleSubEngine->bIsShowSubWindow)
//     {
//         if (ParticleViewerWnd)
//         {
//             ::ShowWindow(ParticleViewerWnd, SW_SHOW);
//         }
//         ParticleSubEngine->bIsShowSubWindow = false;
//     }
// }

// void FEngineLoop::SubEngineControl()
// {
//     OpenParticleSystemViewer();
// }

void FEngineLoop::Exit()
{

    // ParticleSubEngine->Release();
    // CleanupSubWindow();

    LevelEditor->Release();
    UIManager->Shutdown();
    ResourceManager->Release(Renderer);
    Renderer->Release();

    GEngine->Release();

    delete UnrealEditor;
    delete BufferManager;
    delete UIManager;
    delete LevelEditor;
}

// void FEngineLoop::WindowInit(HINSTANCE hInstance)
// {
//     WCHAR WindowClass[] = L"JungleWindowClass";

//     WCHAR Title[] = L"Game Tech Lab";

//     WNDCLASSW Wc{};
//     Wc.lpfnWndProc = AppWndProc;
//     Wc.hInstance = hInstance;
//     Wc.lpszClassName = WindowClass;
//     Wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

//     RegisterClassW(&Wc);

//     AppWnd = CreateWindowExW(
//         0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
//         CW_USEDEFAULT, CW_USEDEFAULT, 1400, 1000,
//         nullptr, nullptr, hInstance, nullptr);
// }

// LRESULT CALLBACK FEngineLoop::AppWndProc(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam)
// {
//     if (hWnd == GEngineLoop.AppWnd)
//     {
//         ImGui::SetCurrentContext(GEngineLoop.UIManager->GetContext());
//         if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
//             return true;
//     }
//     else if (hWnd == GEngineLoop.ParticleViewerWnd)
//     {
//         ImGui::SetCurrentContext(GEngineLoop.ParticleSubEngine->SubUI->Context);

//         if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
//             return true;

//         /** SubWindow Msg */
//         switch (Msg)
//         {
//         case WM_SIZE:
//             if (wParam != SIZE_MINIMIZED)
//             {
//                 RECT ClientRect;
//                 GetClientRect(hWnd, &ClientRect);

//                 float FullWidth = static_cast<float>(ClientRect.right - ClientRect.left);
//                 float FullHeight = static_cast<float>(ClientRect.bottom - ClientRect.top);

//                 if (GEngineLoop.GetUnrealEditor())
//                 {
//                     GEngineLoop.ParticleViewerGD->Resize(hWnd, FullWidth, FullHeight);
//                     GEngineLoop.GetUnrealEditor()->OnResize(hWnd, EWindowType::WT_ParticleSubWindow);
//                 }
//                 GEngineLoop.ParticleSubEngine->ViewportClient->AspectRatio = (FullWidth * 0.75f) / FullHeight;
//             }
//             return 0;
//         case WM_CLOSE:
//             GEngineLoop.ParticleSubEngine->ViewportClient->CameraReset();
//             GEngineLoop.ParticleSubEngine->RequestShowWindow(false);
//             ::ShowWindow(hWnd, SW_HIDE);
//             return 0;

//         case WM_ACTIVATE:
//             if (ImGui::GetCurrentContext() == nullptr)
//                 break;
//             ImGui::SetCurrentContext(GEngineLoop.ParticleSubEngine->SubUI->Context);
//             GEngineLoop.CurrentImGuiContext = ImGui::GetCurrentContext();
//             return 0;
//         case WM_KEYDOWN:
//         case WM_LBUTTONDOWN:
//         case WM_RBUTTONDOWN:
//             ::SetFocus(hWnd);
//             break;
//         default:
//             return DefWindowProc(hWnd, Msg, wParam, lParam);
//         }
//     }

//     switch (Msg)
//     {
//     case WM_DESTROY:
//         PostQuitMessage(0);
//         if (auto LevelEditor = GEngineLoop.GetLevelEditor())
//         {
//             LevelEditor->SaveConfig();
//         }
//         GEngineLoop.bIsExit = true;
//         break;
//     case WM_SIZE:
//         if (wParam != SIZE_MINIMIZED)
//         {
//             if (auto LevelEditor = GEngineLoop.GetLevelEditor())
//             {
//                 GEngineLoop.GraphicDevice->Resize(hWnd);
//                 // FEngineLoop::Renderer->DepthPrePass->ResizeDepthStencil();

//                 uint32 ClientWidth = 0;
//                 uint32 ClientHeight = 0;
//                 GEngineLoop.GetClientSize(ClientWidth, ClientHeight);

//                 LevelEditor->ResizeEditor(ClientWidth, ClientHeight);
//                 // FEngineLoop::Renderer->TileLightCullingPass->ResizeViewBuffers(
//                 //   static_cast<uint32>(LevelEditor->GetActiveViewportClient()->GetD3DViewport().Width),
//                 //     static_cast<uint32>(LevelEditor->GetActiveViewportClient()->GetD3DViewport().Height)
//                 // );
//             }
//         }
//         GEngineLoop.UpdateUI();
//         break;
//     case WM_ACTIVATE:
//         if (ImGui::GetCurrentContext() == nullptr)
//             break;
//         ImGui::SetCurrentContext(GEngineLoop.UIManager->GetContext());
//         GEngineLoop.CurrentImGuiContext = ImGui::GetCurrentContext();
//         break;
//     default:
//         if (hWnd == GEngineLoop.AppWnd && GEngineLoop.AppMessageHandler != nullptr)
//         {
//             FWindowsSlateAppMessageHandler *WindowsHandler = static_cast<FWindowsSlateAppMessageHandler *>(GEngineLoop.AppMessageHandler);
//             WindowsHandler->ProcessMessage(hWnd, Msg, wParam, lParam);
//         }
//         return DefWindowProc(hWnd, Msg, wParam, lParam);
//     }

//     return 0;
// }

void FEngineLoop::InitApplication(FGenericApplicationInitParams* InAppInitParams)
{
    char ApplicationName[] = "GraphicsPlayground";
    wchar_t ApplicationNameW[] = L"GraphicsPlayground";

#ifdef BUILD_PLATFORM_WINDOWS
    FWindowsApplicationInitParams* WindowsAppInitParams = static_cast<FWindowsApplicationInitParams*>(InAppInitParams);
    FWindowsApplication* WindowsApplication = new FWindowsApplication(WindowsAppInitParams);
    Application = WindowsApplication;

    std::shared_ptr<FWindowsWindowInitParams> WindowParams = std::make_shared<FWindowsWindowInitParams>();
    WindowParams->Title = ApplicationNameW;
    WindowParams->hInstance = WindowsAppInitParams->hInstance;

    MainWindow = Application->MakeWindow(WindowParams);
    Application->RegisterMainWindow(MainWindow);
#elif defined(BUILD_PLATFORM_MACOS)
    // Initialize OpenGL RHI first to get the native window handle

    FMacApplicationInitParams* MacAppInitParams = static_cast<FMacApplicationInitParams*>(InAppInitParams);
    FMacApplication* MacApplication = new FMacApplication(MacAppInitParams);
    Application = MacApplication;

    FOpenGLDynamicRHI* OpenGLRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
    FPlatformOpenGLDevice* OpenGLDevice = static_cast<FPlatformOpenGLDevice*>(OpenGLRHI->RHIGetNativeDevice());

    std::shared_ptr<FMacWindowInitParams> MacWindowParams = std::make_shared<FMacWindowInitParams>();
    MacWindowParams->Title = ApplicationName;
    MacWindowParams->WindowHandle = OpenGLDevice->MainContext.Window;

    MainWindow = MacApplication->MakeWindow(MacWindowParams);
    Application->RegisterMainWindow(MainWindow);
#endif
}

void FEngineLoop::UpdateUI()
{
    int32 X, Y, Width, Height;
    MainWindow->GetWindowShape(X, Y, Width, Height);
    // FConsole::GetInstance().OnResize(AppWnd);
    if (GEngineLoop.GetUnrealEditor())
    {
        GEngineLoop.GetUnrealEditor()->OnResize(static_cast<float>(Width), static_cast<float>(Height));
    }
    ViewportTypePanel::GetInstance().Resize(Width, Height);
}

// void FEngineLoop::ParticleSubWindowInit(HINSTANCE hInstance)
// {
//     WCHAR SubWindowClass[] = L"ParticleWindowClass";
//     WCHAR SubTitle[] = L"Viewer";

//     WNDCLASSEXW wcexSub = {}; // WNDCLASSEXW 사용 권장
//     wcexSub.cbSize = sizeof(WNDCLASSEX);
//     wcexSub.style = CS_HREDRAW | CS_VREDRAW; // | CS_DBLCLKS 등 필요시 추가
//     wcexSub.lpfnWndProc = AppWndProc;        // 서브 윈도우 프로시저 지정
//     wcexSub.cbClsExtra = 0;
//     wcexSub.cbWndExtra = 0;
//     wcexSub.hInstance = hInstance;
//     wcexSub.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//     wcexSub.lpszMenuName = nullptr;
//     wcexSub.lpszClassName = SubWindowClass;
//     // wcexSub.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
//     if (!RegisterClassExW(&wcexSub))
//     {
//         // 오류 처리
//         UE_LOG(ELogLevel::Error, TEXT("Failed to register sub window class!"));
//         return;
//     }

//     // 서브 윈도우 생성 (크기, 위치, 스타일 조정 필요)
//     // WS_OVERLAPPEDWINDOW 는 타이틀 바, 메뉴, 크기 조절 등이 포함된 일반적인 창
//     // WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME 등으로 커스텀 가능
//     ParticleViewerWnd = CreateWindowExW(
//         0, SubWindowClass, SubTitle, WS_OVERLAPPEDWINDOW, // WS_VISIBLE 제거 (초기에는 숨김)
//         CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,           // 원하는 크기
//         nullptr,                                          // 부모 윈도우를 메인 윈도우로 설정 (선택 사항)
//         nullptr, hInstance, nullptr);

//     if (!ParticleViewerWnd)
//     {
//         UE_LOG(ELogLevel::Error, TEXT("Failed to create sub window!"));
//     }
//     else
//     {
//     }
// }

// void FEngineLoop::CleanupSubWindow()
// {
//     if (ParticleViewerGD->Device)
//         ParticleViewerGD->Release();
// }
