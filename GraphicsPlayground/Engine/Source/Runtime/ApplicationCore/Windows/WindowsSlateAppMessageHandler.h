#pragma once
#include "ApplicationCore/Generic/GenericSlateAppMessageHandler.h"

class FRawInput;

class FWindowsSlateAppMessageHandler : public FGenericSlateAppMessageHandler
{
public:
    FWindowsSlateAppMessageHandler();
    ~FWindowsSlateAppMessageHandler() = default;

    void ProcessMessage(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam);

    bool IsWindowFocused(void* NativeWindowPtr) const override;
protected:
    void OnRawMouseInput(const RAWMOUSE& RawMouseInput);
    void OnRawKeyboardInput(const RAWKEYBOARD& RawKeyboardInput);

private:
    void HandleRawInput(const RAWINPUT& RawInput);

    std::unique_ptr<FRawInput> RawInputHandler;
};
