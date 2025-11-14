#include "MacApplication.h"
#include "MacWindow.h"
#include "OpenGLDrv/OpenGL3.h"

std::shared_ptr<FGenericWindow> FMacApplication::MakeWindow(std::shared_ptr<FGenericWindowInitParams> Params)
{
    return std::shared_ptr<FMacWindow>(new FMacWindow(Params));
}

void FMacApplication::PumpMessages()
{
    FOpenGL::PollEvents();
    if (FOpenGL::ShouldClose(static_cast<FMacWindow*>(MainWindow.get())->WindowHandle))
    {
        bExitRequested = true;
    }
}
