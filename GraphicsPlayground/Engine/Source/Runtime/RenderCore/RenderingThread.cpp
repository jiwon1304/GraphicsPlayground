/**
 * 게임스레드 - 렌더스레드 - RHI스레드 구조 구현
 * 게임스레드에서 GDynamicRHI로 직접 RHI 함수를 호출하거나
 * GRHICommandList.Get()->EnqueueLambda()를 호출하면 람다함수가 등록됨
 * 등록된 람다함수들은 FRHICommandListBase::CommandList에 의해서 linked list로 저장됨
 * RHI 스레드 (현재는 렌더스레드)에서 FRHICommandListExecutor::Submit()을 호출하면
 * 이것저것 좀더하고서 FRHICommandList::ImmediateFlush()을 호출
 * 이후 쌓여있던 람다함수들이 순차적으로 실행됨
 * 
 */


#include "RenderingThread.h"
#include "Runtime/Core/HAL/Runnable.h"
#include "Runtime/RHI/DynamicRHI.h"
#include "Runtime/RHI/RHICommandList.h"
#include "Core/HAL/RunnableThread.h"
#include "Core/Delegates/DelegateCombination.h"

#if USE_THREADED_RENDERING == 1
static class FRunnableThread* RenderingThread = nullptr;
extern uint32 GRenderingThreadId;

#if USE_RHI_THREAD == 1
static class FRunnableThread* RHIThread = nullptr; // RHIThread is only bound here
extern uint32 GRHIThreadId;
static std::mutex CmdListMutex;
static std::condition_variable CmdListWaitCondition;
static bool bHasPendingCmdLists = false;
#endif // USE_RHI_THREAD
// ####
// 멀티스레드로 하지말고 일단은 mainloop에서 executer가 게임스레드에서 쌓인 커맨드리스트를 실행하게끔 하자
// 일단 scenecomponent들 전부 rhi로 옮기는게 먼저
// ####

static void StartRenderingThread();
static void RenderingThreadMain();

static bool bStopRenderingThread = false;
static bool bStopRHIThread = false;

void StopRenderingThread()
{
    bStopRenderingThread = true;
}

void StopRHIThread()
{
    bStopRHIThread = true;
}
#endif 

bool GUseThreadedRendering = (USE_THREADED_RENDERING == 1);

FRHICommandListImmediate& GetImmediateCommandList_ForRenderCommand()
{
    return FRHICommandListExecutor::GetImmediateCommandList();
}

void InitRenderingThread()
{
#if USE_THREADED_RENDERING == 0
    GUseThreadedRendering = false;
    return;
#else
    StartRenderingThread();
#endif
}

static void ShutdownRenderingThread()
{
#if USE_RHI_THREAD == 0
    return;
#else
    assert (RHIThread);
    RHIThread->Kill();
    delete RHIThread;
    RHIThread = nullptr;

    assert (RenderingThread);
    RenderingThread->Kill();
    delete RenderingThread;
    RenderingThread = nullptr;
#endif
}

bool IsInRenderingThread()
{
#if USE_THREADED_RENDERING == 0
    return true;
#else
    std::thread::id Tid = std::this_thread::get_id();
    uint32 CurrentThreadId = std::hash<std::thread::id>()(Tid);
    return GRenderingThreadId == CurrentThreadId;
#endif
}

bool IsInRHIThread()
{
#if USE_RHI_THREAD == 0
    return true;
#else
    std::thread::id Tid = std::this_thread::get_id();
    uint32 CurrentThreadId = std::hash<std::thread::id>()(Tid);
    return GRHIThreadId == CurrentThreadId;
#endif
}

#if USE_THREADED_RENDERING == 1

/** Rendering thread main loop */
void RenderingThreadMain()
{
    GRenderingThreadId = GetThisThreadId();

    // TODO : delegate 등록하기
    while (!bStopRenderingThread)
    {
        // Rendering work goes here
    }

    GRenderingThreadId = 0;
}

class FRenderingThreadRunnable : public FRunnable
{
    virtual bool Init() override
    {
        return true;
    }

    virtual void Run() override
    {
        RenderingThreadMain();
    }

    virtual void Stop() override
    {
        bStopRenderingThread = true;
    }
};

static void StartRenderingThread()
{
    FRHIThreadRunnable* RHIRunnable = new FRHIThreadRunnable();
    RHIThread = FRunnableThread::Create(std::unique_ptr<FRunnable>(RHIRunnable), TEXT("RHIThread"));
    assert(RHIThread);

    FRenderingThreadRunnable* RenderingRunnable = new FRenderingThreadRunnable();
    RenderingThread = FRunnableThread::Create(std::unique_ptr<FRunnable>(RenderingRunnable), TEXT("RenderingThread"));
    assert(RenderingThread);   

    return;
}

#if USE_RHI_THREAD == 1
/**
 * RHI Thread
 */
class FRHIThreadRunnable : public FRunnable
{
public:
    virtual bool Init() override
    {
        return true;
    }

    virtual void Run() override
    {
        RHIThreadMain();
    }

    virtual void Stop() override
    {
        bStopRHIThread = true;
    }
};

static void ShutdownRHIThread()
{

}

/** RHI thread main loop */
void RHIThreadMain()
{
    GRHIThreadId = GetThisThreadId();

    RHIInit();

    while (!bStopRHIThread)
    {
        {
            // Wait for command lists
            std::unique_lock<std::mutex> Lock(CmdListMutex);
            CmdListWaitCondition.wait(Lock, []() { return bHasPendingCmdLists || bStopRHIThread; });
            bHasPendingCmdLists = false;
        }
        // Execute command lists
        FRHICommandListExecutor::GetImmediateCommandList().ExecuteAndReset();
    }

    GRHIThreadId = 0;
}

#endif // USE_RHI_THREAD
#endif // USE_THREADED_RENDERING