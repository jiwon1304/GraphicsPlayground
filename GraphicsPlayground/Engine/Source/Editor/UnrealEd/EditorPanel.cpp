#include "EditorPanel.h"
#include "ApplicationCore/Generic/GenericWindow.h"

void UEditorPanel::OnResize(const FGenericWindow *Window)
{
    int WindowX, WindowY, WindowWidth, WindowHeight;
    Window->GetWindowShape(WindowX, WindowY, WindowWidth, WindowHeight);
    this->Width = static_cast<float>(WindowWidth);
    this->Height = static_cast<float>(WindowHeight);
}