#pragma once
#include "Components/ActorComponent.h"
#include "UnrealEd/EditorPanel.h"
#include "UnrealEd/EditorViewportClient.h"

struct FRenderTargetRHI;
struct FDepthStencilRHI;

class ParticleViewerPanel : public UEditorPanel
{
public:
    void PrepareRender(FEditorViewportClient* ViewportClient);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override; // HWND는 Windows 특정 타입
private:

    float Width = 800.0f; 
    float Height = 600.0f;

    FRenderTargetRHI* RenderTargetRHI;
    FDepthStencilRHI* DepthStencilRHI;
};

