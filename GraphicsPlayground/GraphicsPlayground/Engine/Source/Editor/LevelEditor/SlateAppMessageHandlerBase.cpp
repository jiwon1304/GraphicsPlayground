#include "SlateAppMessageHandlerBase.h"

FSlateAppMessageHandlerBase::FSlateAppMessageHandlerBase()
{
    for (bool& KeyState : ModifierKeyState)
    {
        KeyState = false;
    }
}

void FSlateAppMessageHandlerBase::OnKeyChar(const TCHAR Character, const bool IsRepeat)
{
    OnKeyCharDelegate.Broadcast(Character, IsRepeat);
}

void FSlateAppMessageHandlerBase::OnKeyDown(const uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat)
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

void FSlateAppMessageHandlerBase::OnKeyUp(const uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat)
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

void FSlateAppMessageHandlerBase::OnMouseDown(const EMouseButtons::Type Button, const FVector2D CursorPos)
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

void FSlateAppMessageHandlerBase::OnMouseUp(const EMouseButtons::Type Button, const FVector2D CursorPos)
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

void FSlateAppMessageHandlerBase::OnMouseDoubleClick(const EMouseButtons::Type Button, const FVector2D CursorPos)
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

void FSlateAppMessageHandlerBase::OnMouseWheel(const float Delta, const FVector2D CursorPos)
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

void FSlateAppMessageHandlerBase::OnMouseMove()
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

