#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

class FRunnable
{
public:
    virtual ~FRunnable()
    {
        Stop();
        if (ThreadPtr && ThreadPtr->joinable())
            ThreadPtr->join();
    }

    // 스레드 시작
    bool StartThread()
    {
        if (!Init())
            return false;

        bStopRequested = false;
        ThreadPtr = std::make_unique<std::thread>(&FRunnable::RunInternal, this);
        return true;
    }

    // 종료 요청
    virtual void Stop()
    {
        bStopRequested = true;
    }

protected:
    virtual bool Init() { return true; }

    // 스레드 본체 - 상속받는 클래스가 구현해야 함
    virtual uint32_t Run() = 0;

    // 종료 시 cleanup
    virtual uint32_t Exit() = 0;

    // 스레드 안전 종료
    void RunInternal()
    {
        Run();
        Exit();
    }

protected:
    std::unique_ptr<std::thread> ThreadPtr;
    std::atomic<bool> bStopRequested{ false };
};
