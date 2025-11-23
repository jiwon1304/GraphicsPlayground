#include "WindowsWindow.h"

void FWindowsWindow::GetWindowShape(int32& X, int32& Y, int32& Width, int32& Height) const
{
    RECT WindowRect;
    GetWindowRect(WindowHandle, &WindowRect);
    X = WindowRect.left;
    Y = WindowRect.top;
    Width = WindowRect.right - WindowRect.left;
    Height = WindowRect.bottom - WindowRect.top;
}

bool FWindowsWindow::IsActive() const
{
    return GetActiveWindow() == WindowHandle;
}

FWindowsWindow::FWindowsWindow(std::shared_ptr<FGenericWindowInitParams> InParams)
    : FGenericWindow(InParams), WindowHandle(nullptr)
{
    FWindowsWindowInitParams* WinParams = static_cast<FWindowsWindowInitParams*>(InParams.get());
    WindowHandle = CreateWindowExW(
        0,
        InParams->Title,
        InParams->Title,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        InParams->InitialWindowRect.GetWidth(),
        InParams->InitialWindowRect.GetHeight(),
        nullptr,
        nullptr,
        WinParams->hInstance,
        nullptr
    );
}
