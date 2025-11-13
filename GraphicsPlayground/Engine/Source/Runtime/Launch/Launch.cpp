#include "Core/HAL/PlatformType.h"
#include "Launch/EngineLoop.h"

#if defined(BUILD_PLATFORM_WINDOWS)
#include "ApplicationCore/Windows/WindowsApplication.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // 사용 안하는 파라미터들
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    FWindowsApplicationInitParams* WindowsAppInitParams = new FWindowsApplicationInitParams();
    WindowsAppInitParams->hInstance = hInstance;

    GEngineLoop.Init(WindowsAppInitParams);
    GEngineLoop.Tick();
    GEngineLoop.Exit();

    delete WindowsAppInitParams;

    return 0;
}
#elif defined(BUILD_PLATFORM_MACOS)
#include "ApplicationCore/Mac/MacApplication.h"
int main(int argc, char** argv)
{
    FMacApplicationInitParams* MacAppInitParams = new FMacApplicationInitParams();
    
    GEngineLoop.Init(MacAppInitParams);
    GEngineLoop.Tick();
    GEngineLoop.Exit();

    delete MacAppInitParams;

    return 0;
}
#else

static_assert(false, "Unsupported platform for entry point");

#endif