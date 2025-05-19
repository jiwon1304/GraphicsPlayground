#pragma once
#include "Components/ActorComponent.h"
#include "UnrealEd/EditorPanel.h"
#include "UnrealEd/EditorViewportClient.h"

struct FRenderTargetRHI;
struct FDepthStencilRHI;

class UParticleSystem;
class UParticleEmitter;
class ParticleViewerPanel : public UEditorPanel
{
public:
    void PrepareRender(FEditorViewportClient* ViewportClient);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override; // HWND는 Windows 특정 타입

private:
    void RenderPanelLayout();
    void RenderFilePanel();
    void RenderViewportPanel();
    void RenderEmitterPanel();
    void RenderDetailPanel();
    void RenderCurveEditorPanel();

private://Emitter Panel
    void RenderEmitterModulePopup(int EmitterIndex);
    void RenderEmitterCreatePopup();
    void InputEmitterPanel();
private://File Panel
    void RenderParticleSystemList();
    void RenderCreateParticlePopup();
    void CreateNewParticleSystem(const FString& Name);
    void RemoveParticleSystem(const FName& AssetName);
private:
    UParticleEmitter* CreateDefaultParticleEmitter();
private:
    int SelectedEmitterIndex = -1;
    float ItemWidth = 200.0f;   // 각 Emitter 블록의 가로 크기
    float WidthPad = 10.0f;      // Emitter 넓이 간격
    float HeightPad = 20.0f;      // Emitter 높이 간격

    ImVec4 EmitterBGColor = ImVec4(0.07843f, 0.07843f, 0.09804f, 1.0f); // Emitter 블록 배경 색상
    ImVec4 SelectedEmitterBGColor = ImVec4(0.2f, 0.6f, 0.8f, 1.0f); // 선택된 Emitter 블록 배경 색상
    ImVec4 EmitterTextColor = ImVec4(0.7059f, 0.7059f, 0.7059f, 1.0f); // Emitter 블록 텍스트 색상
    ImVec4 SelectedEmitterTextColor = ImVec4(1.0f, 0.39216f, 0.0f, 1.0f); // 선택된 Emitter 블록 텍스트 색상
private:

    float Width = 800.0f; 
    float Height = 600.0f;

    FRenderTargetRHI* RenderTargetRHI;
    FDepthStencilRHI* DepthStencilRHI;

private:
    UParticleSystem* ParticleSystem = nullptr;
private:
    int CurrentIndex = 0;
    TArray<FName> ParticleNames;
};

