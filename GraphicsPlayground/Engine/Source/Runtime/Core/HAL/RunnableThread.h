#pragma once
#include "Runnable.h"

#include <thread>
#include <atomic>
#include <memory>
#include <chrono>
#include <cassert>

#include "HAL/PlatformType.h"
#include "Core/Container/String.h"

/**
 * @brief Immediately runs a runnable object in its own thread.
 * Runs an FRunnable object in its own thread.
 * Destructor of FRunnable will be called in this class's destructor.
 * The destructor will wait for the thread to complete.
 * You can also call Join() to call the FRunnable destructor in advance.
 */
class FRunnableThread
{
public:
    static FRunnableThread* Create(std::unique_ptr<FRunnable> InRunnable, const TCHAR* InThreadName);

    virtual ~FRunnableThread()
    {
        // Thread is still running
        assert(Thread.joinable() || bFinished.load(std::memory_order_acquire) || !bIsRunning.load(std::memory_order_acquire));
    }

    // @todo
    // virtual void Suspend(bool bShouldPause = true) = 0;

    /**
     * Non blocking query
     */
    virtual bool IsComplete() const;
    
    /**
     * Waits for the thread to complete the execution.
     */
    void Wait();
    
    /**
     * Wait for the thread to complete and explicitly call the FRunnable destructor.
     */
    void Join();

    /**
     * @brief Call FRunnable::Stop() and wait for the thread to complete.
     * 
     * FRunnable::Stop() should be implemented properly in the derived class.
     * Otherwise, this function just waits for the thread to complete.
     * This function also calls FRunnable::Join() after completion.
     */
    virtual void Kill(/* bool bShouldWait = false */);

    uint32 GetThreadID() const
	{
		return ThreadID;
	}

protected:
    FString ThreadName;

    uint32 ThreadID;

    FRunnableThread(std::unique_ptr<FRunnable> InRunnable, const TCHAR* InThreadName);

private:
    std::unique_ptr<FRunnable> Runnable;
    std::thread Thread;
    /**
     * We use atomic with c++20 wait/notify instead of mutex+condition_variable.
     */
    std::atomic<bool> bIsRunning;
    std::atomic<bool> bFinished;
};

extern uint32 GetThisThreadId();
