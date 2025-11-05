#include "MacWindow.h"
#include "OpenGLDrv/OpenGL3.h"

void FMacWindow::ReshapeWindow(int32 X, int32 Y, int32 Width, int32 Height)
{
    FOpenGL::SetWindowSize(WindowHandle, Width, Height);
    FOpenGL::SetWindowPos(WindowHandle, X, Y);
}

void FMacWindow::GetWindowShape(int32 &X, int32 &Y, int32 &Width, int32 &Height) const
{
    FOpenGL::GetWindowPos(WindowHandle, &X, &Y);
    FOpenGL::GetWindowSize(WindowHandle, &Width, &Height);
}

// void FMacWindow::Activate()
// {
//     FOpenGL::FocusWindow(WindowHandle);
// }

bool FMacWindow::IsActive() const
{
    return FOpenGL::IsWindowFocused(WindowHandle);
}

FMacWindow::FMacWindow(std::shared_ptr<FGenericWindowInitParams> Params) : FGenericWindow(Params)
{
    FMacWindowInitParams* MacParams = static_cast<FMacWindowInitParams*>(Params.get());
    WindowHandle = MacParams->WindowHandle;
}