#pragma once
#include "Classes/Components/ActorComponent.h"
#include "Editor/UnrealEd/EditorPanel.h"

class OutlinerEditorPanel : public UEditorPanel
{
public:
    OutlinerEditorPanel();

public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;
    
private:
    float Width = 0, Height = 0;
};
