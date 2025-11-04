#include "RunnableThread.h"

uint32 GetCurrentThreadId()
{
    std::thread::id Tid = std::this_thread::get_id();
    return std::hash<std::thread::id>()(Tid);
}

FRunnableThread* FRunnableThread::Create(std::unique_ptr<FRunnable> InRunnable, const TCHAR* InThreadName)
{
    return new FRunnableThread(std::move(InRunnable), InThreadName);
}

bool FRunnableThread::IsComplete() const
{
    return bFinished.load(std::memory_order_acquire);
}

void FRunnableThread::Wait()
{
    bFinished.wait(false);
}

void FRunnableThread::Join()
{
    Wait();
    assert(Thread.joinable());
    Thread.join();
}

void FRunnableThread::Kill(/* bool bShouldWait */)
{
    assert(Runnable);
    Runnable->Stop();
    Join();
}

FRunnableThread::FRunnableThread(std::unique_ptr<FRunnable> InRunnable, const TCHAR *InThreadName)
    : ThreadName(InThreadName), ThreadID(0), Runnable(std::move(InRunnable)), Thread(), bIsRunning(false), bFinished(false)
{
    // Init in the caller thread context
    if (Runnable && !Runnable->Init())
    {
        // Initialization failed; do not start the thread
        bFinished.store(true, std::memory_order_release);
        return;
    }

    Thread = std::thread([this]() {
        ThreadID = GetCurrentThreadId();
        bIsRunning.store(true, std::memory_order_release);

        assert(Runnable);
        Runnable->Run();

        bIsRunning.store(false, std::memory_order_release);
        bFinished.store(true, std::memory_order_release);
        bFinished.notify_all();
    });
}
