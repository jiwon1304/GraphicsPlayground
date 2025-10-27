#pragma once
#include "Editor/LevelEditor/SlateAppMessageHandlerBase.h"

class FRawInput;

class FSlateAppMessageHandlerWindows : public FSlateAppMessageHandlerBase
{
public:
    FSlateAppMessageHandlerWindows();
    ~FSlateAppMessageHandlerWindows() = default;

    void ProcessMessage(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam);

protected:
    void OnRawMouseInput(const RAWMOUSE& RawMouseInput);
    void OnRawKeyboardInput(const RAWKEYBOARD& RawKeyboardInput);

private:
    void HandleRawInput(const RAWINPUT& RawInput);

    std::unique_ptr<FRawInput> RawInputHandler;
};
