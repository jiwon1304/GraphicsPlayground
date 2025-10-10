#pragma once
#include "Core/Delegates/DelegateCombination.h"
#include "HAL/PlatformType.h"
#include "InputCore/InputCoreTypes.h"
#include "Math/Vector.h"
#include "SlateCore/Input/Events.h"

namespace EMouseButtons
{
enum Type : uint8;
}

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeyCharDelegate, const TCHAR /*Character*/, const bool /*IsRepeat*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeyDownDelegate, const FKeyEvent& /*InKeyEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeyUpDelegate, const FKeyEvent& /*InKeyEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseDownDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseUpDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseDoubleClickDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseWheelDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseMoveDelegate, const FPointerEvent& /*InMouseEvent*/);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRawMouseInputDelegate, const FPointerEvent& /*InRawMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRawKeyboardInputDelegate, const FKeyEvent& /*InRawKeyboardEvent*/);

DECLARE_MULTICAST_DELEGATE(FOnPIEModeStart);
DECLARE_MULTICAST_DELEGATE(FOnPIEModeEnd);

class FSlateAppMessageHandlerBase
{
public:
    /**
     * Delegates
     */
    FOnKeyCharDelegate OnKeyCharDelegate;
    FOnKeyDownDelegate OnKeyDownDelegate;
    FOnKeyUpDelegate OnKeyUpDelegate;
    FOnMouseDownDelegate OnMouseDownDelegate;
    FOnMouseUpDelegate OnMouseUpDelegate;
    FOnMouseDoubleClickDelegate OnMouseDoubleClickDelegate;
    FOnMouseWheelDelegate OnMouseWheelDelegate;
    FOnMouseMoveDelegate OnMouseMoveDelegate;

    FOnRawMouseInputDelegate OnRawMouseInputDelegate;
    FOnRawKeyboardInputDelegate OnRawKeyboardInputDelegate;

    FOnPIEModeStart OnPIEModeStartDelegate;
    FOnPIEModeEnd OnPIEModeEndDelegate;

public:
    FSlateAppMessageHandlerBase();
    ~FSlateAppMessageHandlerBase() = default;

    void OnPIEModeStart() { OnPIEModeStartDelegate.Broadcast(); }
    void OnPIEModeEnd() { OnPIEModeEndDelegate.Broadcast(); }
protected:
    /**
     * Handles input before broadcast to delegates
     * @TODO : remove actual keys
     */
    void OnKeyChar(const TCHAR Character, const bool IsRepeat);
    void OnKeyDown(uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat);
    void OnKeyUp(uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat);
    void OnMouseDown(const EMouseButtons::Type Button, const FVector2D CursorPos);
    void OnMouseUp(const EMouseButtons::Type Button, const FVector2D CursorPos);
    void OnMouseDoubleClick(const EMouseButtons::Type Button, const FVector2D CursorPos);
    void OnMouseWheel(const float Delta, const FVector2D CursorPos);
    void OnMouseMove();

    /** Cursor와 관련된 변수를 업데이트 합니다. */
    FORCEINLINE void UpdateCursorPosition(const FVector2D& NewPos)
    {
        PreviousPosition = CurrentPosition;
        CurrentPosition = NewPos;
    }

    /** 현재 마우스 포인터의 위치를 가져옵니다. */
    FORCEINLINE FVector2D GetCursorPos() const { return CurrentPosition; }

    /** 한 프레임 전의 마우스 포인터의 위치를 가져옵니다. */
    FORCEINLINE FVector2D GetLastCursorPos() const { return PreviousPosition; }

    // TODO : use bitshift
    /** ModifierKeys의 상태를 가져옵니다. */
    FORCEINLINE FModifierKeysState GetModifierKeys() const
    {
        return FModifierKeysState{
            ModifierKeyState[EModifierKey::LeftShift],
            ModifierKeyState[EModifierKey::RightShift],
            ModifierKeyState[EModifierKey::LeftControl],
            ModifierKeyState[EModifierKey::RightControl],
            ModifierKeyState[EModifierKey::LeftAlt],
            ModifierKeyState[EModifierKey::RightAlt],
            ModifierKeyState[EModifierKey::LeftWin],
            ModifierKeyState[EModifierKey::RightWin],
            ModifierKeyState[EModifierKey::CapsLock]
        };
    }

public:
    FORCEINLINE void BroadcastPIEModeStart()
    {
        OnPIEModeStartDelegate.Broadcast();
    }
    FORCEINLINE void BroadcastPIEModeEnd()
    {
        OnPIEModeEndDelegate.Broadcast();
    }
    
protected:
    struct EModifierKey
    {
        enum Type : uint8
        {
            LeftShift,    // VK_LSHIFT
            RightShift,   // VK_RSHIFT
            LeftControl,  // VK_LCONTROL
            RightControl, // VK_RCONTROL
            LeftAlt,      // VK_LMENU
            RightAlt,     // VK_RMENU
            LeftWin,      // VK_LWIN
            RightWin,     // VK_RWIN
            CapsLock,     // VK_CAPITAL
            Count,
        };
    };

    // Cursor Position
    FVector2D CurrentPosition;
    FVector2D PreviousPosition;

    bool ModifierKeyState[EModifierKey::Count];
    TSet<EKeys::Type> PressedMouseButtons;
};
