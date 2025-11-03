#include "ParticleSubEngine.h"
#include "Launch/ImGuiManager.h"
#include "Windows/SubWindow/ImGuiSubWindow.h"
#include "SubRenderer.h"
#include "Engine/UnrealClient.h"
#include "Classes/Actors/Cube.h"
#include "Classes/Animation/Skeleton.h"
#include "Classes/Engine/Asset/AssetManager.h"
#include "Editor/PropertyEditor/SubEditor/ParticleViewerPanel.h"
#include "Classes/Particles/ParticleSystem.h"
#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Particles/ParticleModules/ParticleModuleRequired.h"
#include "Classes/Components/ParticleSystemComponent.h"
#include "Windows/D3D11RHI/GraphicDevice.h"

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
            ViewportClient->CameraMoveRight(-CameraSpeedMultiplier * DeltaTime);
        }
        if (GetAsyncKeyState('D') & 0x8000)
        {
            ViewportClient->CameraMoveRight(CameraSpeedMultiplier * DeltaTime);
        }
        if (GetAsyncKeyState('W') & 0x8000)
        {
            ViewportClient->CameraMoveForward(CameraSpeedMultiplier * DeltaTime);
        }
        if (GetAsyncKeyState('S') & 0x8000)
        {
            ViewportClient->CameraMoveForward(-CameraSpeedMultiplier * DeltaTime);
        }
        if (GetAsyncKeyState('E') & 0x8000)
        {
            ViewportClient->CameraMoveUp(CameraSpeedMultiplier * DeltaTime);
        }
        if (GetAsyncKeyState('Q') & 0x8000)
        {
            ViewportClient->CameraMoveUp(-CameraSpeedMultiplier * DeltaTime);
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
}

UParticleSystemComponent* UParticleSubEngine::GetParticleSystemComponent() const
{
    return ParticleSystemComponent;
}
