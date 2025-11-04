#pragma once

#include "HAL/PlatformType.h"

/**
 * FRunnable is an interface for creating runnable objects that can be executed in their own thread.
 * It provides methods for initialization, execution, stopping, and cleanup.
 */
class FRunnable
{
public:
	/**
	 * Called in the caller thread context.
	 * Do not create threads if returns false.
	 */
	virtual bool Init()
	{
		return true;
	}

	/**
	 * Run the runnable object.
	 * Called in the runnable thread context.
	 */
	virtual void Run() = 0;

    /**
     * Stop the runnable object early.
     * Called by RunnableThread::Kill() method.
	 * Run() should handle this properly and exit as soon as possible.
     */
	virtual void Stop() { }

    // /**
	//  * Exits the runnable object.
	//  * Called in the runnable thread context.
    //  */
	// virtual void Exit() { }

	/** 
	 * Called in the destructor of FRunnableThread
	 */
	virtual ~FRunnable() { }
};
