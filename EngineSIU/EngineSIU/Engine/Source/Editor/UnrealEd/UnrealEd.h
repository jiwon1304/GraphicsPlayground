#pragma once
#include "Container/Map.h"
#include "Container/String.h"
#include "EditorPanel.h"

class UEditorPanel;

class UnrealEd
{
public:
    UnrealEd() = default;
    ~UnrealEd() = default;
    void Initialize();

    void Render(EWindowType WindowType = EWindowType::WT_Main) const;
     void OnResize(HWND hWnd, EWindowType WindowType = EWindowType::WT_Main) const;
    
    void AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel);
    std::shared_ptr<UEditorPanel> GetEditorPanel(const FString& PanelId);

private:
    TMap<FString, std::shared_ptr<UEditorPanel>> Panels;
    TMap<FString, std::shared_ptr<UEditorPanel>> ParticleSubPanels;
};
