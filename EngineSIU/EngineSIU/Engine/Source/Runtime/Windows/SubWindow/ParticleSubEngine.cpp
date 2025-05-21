#include "ParticleSubEngine.h"
#include "ImGuiManager.h"
#include "ImGuiSubWindow.h"
#include "SubRenderer.h"
#include "UnrealClient.h"
#include "Actors/Cube.h"
#include "Animation/Skeleton.h"
#include "Engine/AssetManager.h"
#include "PropertyEditor/SubEditor/ParticleViewerPanel.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModules/ParticleModuleRequired.h"
#include "Components/ParticleSystemComponent.h"
UParticleSubEngine::UParticleSubEngine()
{
}

UParticleSubEngine::~UParticleSubEngine()
{
}

void UParticleSubEngine::Initialize(HWND& hWnd, FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, UImGuiManager* InSubWindow,
    UnrealEd* InUnrealEd)
{
    Super::Initialize(hWnd, InGraphics, InBufferManager, InSubWindow, InUnrealEd);

    EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>(this);
    EditorPlayer->SetCoordMode(CDM_LOCAL);

    ParticleSystemComponent = FObjectFactory::ConstructObject<UParticleSystemComponent>(this);
    ParticleViewerPanel* particlePanel = reinterpret_cast<ParticleViewerPanel*>(UnrealEditor->GetParticleSubPanel("ParticleViewerPanel").get());
    particlePanel->SetParticleSystemComponent(ParticleSystemComponent);
    particlePanel->SetViewportClient(ViewportClient);
}

void UParticleSubEngine::Tick(float DeltaTime)
{
    Input(DeltaTime);
    ViewportClient->Tick(DeltaTime);
    if (ParticleSystemComponent->Template)
    {
        ParticleSystemComponent->TickComponent(DeltaTime);
    }
    Render();
}

void UParticleSubEngine::Input(float DeltaTime)
{
    if (::GetFocus() != *Wnd)
        return;
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
    {
        if (!bRBClicked)
        {
            bRBClicked = true;
            GetCursorPos(&LastMousePos);
        }
        POINT CursorPos;
        GetCursorPos(&CursorPos);

        float DeltaX = CursorPos.x - LastMousePos.x;
        float DeltaY = CursorPos.y - LastMousePos.y;
        ViewportClient->CameraRotateYaw(DeltaX * 0.1f);
        ViewportClient->CameraRotatePitch(DeltaY * 0.1f);
        LastMousePos = CursorPos;
    }
    else
    {
        if (bRBClicked)
        {
            bRBClicked = false;
        }
    }
    if (bRBClicked)
    {
        if (GetAsyncKeyState('A') & 0x8000)
        {
            ViewportClient->CameraMoveRight(-100.f * DeltaTime);
        }
        if (GetAsyncKeyState('D') & 0x8000)
        {
            ViewportClient->CameraMoveRight(100.f * DeltaTime);
        }
        if (GetAsyncKeyState('W') & 0x8000)
        {
            ViewportClient->CameraMoveForward(100.f * DeltaTime);
        }
        if (GetAsyncKeyState('S') & 0x8000)
        {
            ViewportClient->CameraMoveForward(-100.f * DeltaTime);
        }
        if (GetAsyncKeyState('E') & 0x8000)
        {
            ViewportClient->CameraMoveUp(100.f * DeltaTime);
        }
        if (GetAsyncKeyState('Q') & 0x8000)
        {
            ViewportClient->CameraMoveUp(-100.f * DeltaTime);
        }
    }
    else
    {
        if (GetAsyncKeyState('W') & 0x8000)
        {
            EditorPlayer->SetMode(CM_TRANSLATION);
        }
        if (GetAsyncKeyState('E') & 0x8000)
        {
            EditorPlayer->SetMode(CM_ROTATION);
        }
        if (GetAsyncKeyState('R') & 0x8000)
        {
            EditorPlayer->SetMode(CM_SCALE);
        }
    }
}

void UParticleSubEngine::Render()
{
    if (Wnd && IsWindowVisible(*Wnd) && Graphics->Device)
    {
        Graphics->Prepare();

        SubRenderer->PrepareRender(ViewportClient);
        SubRenderer->Render(ViewportClient);
        SubRenderer->ClearRender();
        // Sub window rendering

        SubUI->BeginFrame();

        //UI를 위한 렌더 타겟 설정
        //FGraphicsDevice* Graphics = &FEngineLoop::ParticleViewerGD;
        Graphics->DeviceContext->OMSetRenderTargets(
            1,
            &Graphics->BackBufferRTV,
            Graphics->DeviceDSV
        );

        UnrealEditor->Render(EWindowType::WT_ParticleSubWindow);
        SubUI->EndFrame();

        // Sub swap
        Graphics->SwapBuffer();
    }
}

void UParticleSubEngine::Release()
{
    USubEngine::Release();
    if (SubUI)
    {
        SubUI->Shutdown();
        delete SubUI;
        SubUI = nullptr;
    }
    if (SubRenderer)
    {
        SubRenderer->Release();
        delete SubRenderer;
        SubRenderer = nullptr;
    }
}

UParticleSystemComponent* UParticleSubEngine::GetParticleSystemComponent() const
{
    return ParticleSystemComponent;
}
