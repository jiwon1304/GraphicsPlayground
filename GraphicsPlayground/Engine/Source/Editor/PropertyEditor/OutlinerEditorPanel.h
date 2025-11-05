#pragma once
#include "Classes/Components/ActorComponent.h"
#include "Editor/UnrealEd/EditorPanel.h"

class OutlinerEditorPanel : public UEditorPanel
{
public:
    OutlinerEditorPanel();

public:
    virtual void Render() override;
};
