#include "RenderingThread.h"
#include "Runtime/Core/HAL/Runnable.h"
#include "Runtime/RHI/DynamicRHI.h"
#include "Runtime/RHI/RHICommandList.h"

extern class FRenderingThread* GRenderingThread;
extern uint32 GRenderThreadId;
static class FRHIThread* GRHIThread = nullptr;
extern uint32 GRHIThreadId;

static void StartRenderingThread();
static void RenderingThreadMain();

/**
 * RHI Thread
 */
class FRHIThread : public FRunnable
{
    virtual bool Init() override
    {
        try
        {
            std::thread::id Tid = std::this_thread::get_id();
            GRHIThreadId = std::hash<std::thread::id>()(Tid);
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
    }

    virtual uint32 Run() override
    {
        RenderingThreadMain();
        return 0;
    }

    virtual uint32 Exit() override
    {
        GRHIThreadId = 0;
        return 0;
    }

};

/**
 * Rendering Thread.
 * FRenderingThread이랑 FRenderingThreadRunnable의 차이를 몰라서 그냥 이렇게 사용
 * UE5에서는 rendering thread가 task system에서 관리하는 듯 함.
 */
class FRenderingThread : public FRunnable
{
    virtual bool Init() override
    {
        try
        {
            std::thread::id Tid = std::this_thread::get_id();
            GRenderThreadId = std::hash<std::thread::id>()(Tid);
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
    }
    virtual uint32 Run() override
    {
        RenderingThreadMain();
        return 0;
    }

    virtual uint32 Exit() override
    {
        GRenderThreadId = 0;
        return 0;
    }

}; // GRenderingThread is declared in ThreadingBase.cpp


FRHICommandListImmediate& GetImmediateCommandList_ForRenderCommand()
{
    return FRHICommandListExecutor::GetImmediateCommandList();
}

static void InitRenderingThread()
{
    StartRenderingThread();
}

static void ShutdownRenderingThread()
{

}

/** Rendering thread main loop */
void RenderingThreadMain()
{
    bool bStopThread = false;
    // TODO : delegate 등록하기
    while (!bStopThread)
    {
        
    }
}

static void StartRenderingThread()
{
    assert(GRHIThread == nullptr);
    GRHIThread = new FRHIThread();

    assert(GRHIThread);
    assert(GRenderingThread);
    GRenderingThread = new FRenderingThread();

    GRHIThread->StartThread();
    GRenderingThread->StartThread();

    return;
}
