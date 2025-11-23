#include "WindowsApplication.h"
#include "WindowsWindow.h"

std::shared_ptr<FGenericWindow> FWindowsApplication::MakeWindow(std::shared_ptr<FGenericWindowInitParams> Params)
{
    FWindowsWindow* Window = new FWindowsWindow(Params);

    return std::shared_ptr<FGenericWindow>(Window);
}

void FWindowsApplication::PumpMessages()
{
    MSG Msg;
    while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Msg); // 키보드 입력 메시지를 문자메시지로 변경
        DispatchMessage(&Msg);  // 메시지를 WndProc에 전달

        if (Msg.message == WM_QUIT)
        {
            bExitRequested = true;
            break;
        }
    }

    // if (!bIsExit && ParticleViewerWnd && IsWindowVisible(ParticleViewerWnd))
    // {
    //     while (PeekMessage(&Msg, ParticleViewerWnd, 0, 0, PM_REMOVE))
    //     {
    //         TranslateMessage(&Msg);
    //         DispatchMessage(&Msg);
    //     }
    // }
}
