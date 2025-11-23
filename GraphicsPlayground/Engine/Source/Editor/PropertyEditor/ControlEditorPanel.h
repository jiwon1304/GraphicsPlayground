#pragma once
#include "Classes/Components/ActorComponent.h"
#include "Editor/UnrealEd/EditorPanel.h"
#include "LightGridGenerator.h"

class ControlEditorPanel : public UEditorPanel
{
public:
    ControlEditorPanel();
    virtual void Render() override;

private:
    void CreateMenuButton(ImVec2 ButtonSize, ImFont* IconFont);
    void CreateModifyButton(ImVec2 ButtonSize, ImFont* IconFont);
    static void CreateFlagButton();
    void CreatePIEButton(ImVec2 ButtonSize, ImFont* IconFont);
    static void CreateSRTButton(ImVec2 ButtonSize);
    void CreateLightSpawnButton(ImVec2 InButtonSize, ImFont* IconFont);
    
private:
    bool bOpenModal = false;
    bool bShowImGuiDemoWindow = false; // 데모 창 표시 여부를 관리하는 변수

    FLightGridGenerator LightGridGenerator;
};

