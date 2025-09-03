#pragma once

// CMAKE에 추가하기!!!!!!!!
/** Enqueues a render command to a render pipe. The default implementation takes a lambda and schedules on the render thread.
 *  Alternative implementations accept either a reference or pointer to an FRenderCommandPipe instance to schedule on an async
 *  pipe, if enabled.
 */


/** Whether the rendering thread should be created or not */
extern bool GUseThreadedRendering;

extern class FRHICommandListImmediate& GetImmediateCommandList_ForRenderCommand();

extern void InitRenderingThread();

extern void ShutdownRenderingThread();

extern void TickRenderingTickables();


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

