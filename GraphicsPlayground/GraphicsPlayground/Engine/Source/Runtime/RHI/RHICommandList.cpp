#include "RHICommandList.h"

namespace RHI
{
FRHICommandListExecutor GRHICommandList;

// https://dev.epicgames.com/documentation/ko-kr/unreal-engine/parallel-rendering-overview-for-unreal-engine
// RHI Command basically does two processes : translation and submission.
// Translation is the process of converting high-level rendering commands into low-level GPU instructions.
// There can be some heuristics in optimizing the command lists.
// Submission is the process of sending the translated commands to the GPU for execution.
// 
// Latest graphics APIs utilize parallelism to execute multiple commands concurrently.
// That is, there is one immediate command list and may be other parallel command lists.
// But we only (currently) use D3D11 and OpenGL3.3, which only supports single-thread,
// therefore the translation and submission processes are done sequentially.
void FRHICommandListExecutor::Submit()
{
    // submit all commands in the immediate command list
    CommandListImmediate.ExecuteAndReset();
}

inline FRHICommandListImmediate &FRHICommandListImmediate::Get()
{
    return FRHICommandListExecutor::GetImmediateCommandList();
}

}
