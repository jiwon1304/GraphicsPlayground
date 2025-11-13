#pragma once

#define USE_THREADED_RENDERING 0
#define USE_RHI_THREAD 0

/** Whether the rendering thread should be created or not */
extern bool GUseThreadedRendering;

extern class FRHICommandListImmediate& GetImmediateCommandList_ForRenderCommand();

extern void InitRenderingThread();

extern void ShutdownRenderingThread();

extern bool IsInRenderingThread();

extern bool IsInRHIThread();

/**
 * Originally the rendering commands are "recorded" from game thread and "replayed" in rendering thread.
 * Recording use task system and there can be overhead in creating tasks.
 * This is mitigated by using a render command pipe, which batches commands and reduces the number of tasks created.
 * 
 * This project does not use task system, therefore we just enqueue to global render commandlist.
 * 
 * @param Lambda The parameter should be only (FRHICommandList)
 */
#define ENQUEUE_RENDER_COMMAND(Lambda) \
    GetImmediateCommandList_ForRenderCommand().EnqueueLambda(Lambda)

