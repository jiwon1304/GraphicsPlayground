#include "GenericSlateAppMessageHandler.h"

FGenericSlateAppMessageHandler::FGenericSlateAppMessageHandler()
    : CurrentPosition(FIntPoint())
    , PreviousPosition(FIntPoint())
{
    for (bool& KeyState : ModifierKeyState)
    {
        KeyState = false;
    }
}

void FGenericSlateAppMessageHandler::OnKeyChar(const TCHAR Character, const bool IsRepeat)
{
    OnKeyCharDelegate.Broadcast(Character, IsRepeat);
}

void FGenericSlateAppMessageHandler::OnKeyDown(const uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat)
{
    FInputKeyManager::Get();
    OnKeyDownDelegate.Broadcast(FKeyEvent{
        EKeys::Invalid, // TODO: 나중에 FInputKeyManager구현되면 바꾸기
        GetModifierKeys(),
        IsRepeat ? IE_Repeat : IE_Pressed,
        CharacterCode,
        // KeyCode,
    });
}

void FGenericSlateAppMessageHandler::OnKeyUp(const uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat)
{
    assert(!IsRepeat);  // KeyUp 이벤트에서 IsRepeat가 true일수가 없기 때문에

    OnKeyUpDelegate.Broadcast(FKeyEvent{
        EKeys::Invalid, // TODO: 나중에 FInputKeyManager구현되면 바꾸기
        GetModifierKeys(),
        IE_Released,
        CharacterCode,
        // KeyCode,
    });
}

void FGenericSlateAppMessageHandler::OnMouseDown(const EMouseButtons::Type Button, const FIntPoint CursorPos)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
    EKeys::Type EffectingButton = EKeys::Invalid;
    switch (Button)
    {
    case EMouseButtons::Left:
        EffectingButton = EKeys::LeftMouseButton;
        break;
    case EMouseButtons::Middle:
        EffectingButton = EKeys::MiddleMouseButton;
        break;
    case EMouseButtons::Right:
        EffectingButton = EKeys::RightMouseButton;
        break;
    case EMouseButtons::Thumb01:
        EffectingButton = EKeys::ThumbMouseButton;
        break;
    case EMouseButtons::Thumb02:
        EffectingButton = EKeys::ThumbMouseButton2;
        break;
    case EMouseButtons::Invalid:
        EffectingButton = EKeys::Invalid;
        break;
    }

    PressedMouseButtons.Add(EffectingButton);
    OnMouseDownDelegate.Broadcast(FPointerEvent{
        CursorPos,
        GetLastCursorPos(),
        0.0f,
        EffectingButton,
        PressedMouseButtons,
        GetModifierKeys(),
        IE_Pressed,
    });
}

void FGenericSlateAppMessageHandler::OnMouseUp(const EMouseButtons::Type Button, const FIntPoint CursorPos)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
    EKeys::Type EffectingButton = EKeys::Invalid;
    switch (Button)
    {
    case EMouseButtons::Left:
        EffectingButton = EKeys::LeftMouseButton;
        break;
    case EMouseButtons::Middle:
        EffectingButton = EKeys::MiddleMouseButton;
        break;
    case EMouseButtons::Right:
        EffectingButton = EKeys::RightMouseButton;
        break;
    case EMouseButtons::Thumb01:
        EffectingButton = EKeys::ThumbMouseButton;
        break;
    case EMouseButtons::Thumb02:
        EffectingButton = EKeys::ThumbMouseButton2;
        break;
    case EMouseButtons::Invalid:
        EffectingButton = EKeys::Invalid;
        break;
    }

    PressedMouseButtons.Remove(EffectingButton);
    OnMouseUpDelegate.Broadcast(FPointerEvent{
        CursorPos,
        GetLastCursorPos(),
        0.0f,
        EffectingButton,
        PressedMouseButtons,
        GetModifierKeys(),
        IE_Released,
    });
}

void FGenericSlateAppMessageHandler::OnMouseDoubleClick(const EMouseButtons::Type Button, const FIntPoint CursorPos)
{
    EKeys::Type EffectingButton = EKeys::Invalid;
    switch (Button)
    {
    case EMouseButtons::Left:
        EffectingButton = EKeys::LeftMouseButton;
        break;
    case EMouseButtons::Middle:
        EffectingButton = EKeys::MiddleMouseButton;
        break;
    case EMouseButtons::Right:
        EffectingButton = EKeys::RightMouseButton;
        break;
    case EMouseButtons::Thumb01:
        EffectingButton = EKeys::ThumbMouseButton;
        break;
    case EMouseButtons::Thumb02:
        EffectingButton = EKeys::ThumbMouseButton2;
        break;
    case EMouseButtons::Invalid:
        EffectingButton = EKeys::Invalid;
        break;
    }

    PressedMouseButtons.Add(EffectingButton);
    OnMouseDoubleClickDelegate.Broadcast(FPointerEvent{
        CursorPos,
        GetLastCursorPos(),
        0.0f,
        EffectingButton,
        PressedMouseButtons,
        GetModifierKeys(),
        IE_DoubleClick,
    });
}

void FGenericSlateAppMessageHandler::OnMouseWheel(const float Delta, const FIntPoint CursorPos)
{
    OnMouseWheelDelegate.Broadcast(FPointerEvent{
        CursorPos,
        GetLastCursorPos(),
        Delta,
        EKeys::MouseWheelAxis,
        PressedMouseButtons,
        GetModifierKeys(),
        IE_Axis,
    });
}

void FGenericSlateAppMessageHandler::OnMouseMove()
{
    OnMouseMoveDelegate.Broadcast(FPointerEvent{
        GetCursorPos(),
        GetLastCursorPos(),
        0.0f,
        EKeys::Invalid,
        PressedMouseButtons,
        GetModifierKeys(),
        IE_Axis,
    });
}

