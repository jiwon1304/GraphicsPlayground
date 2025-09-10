#ifndef INTERNAL_DECORATOR
#define INTERNAL_DECORATOR(Method) CmdList.GetContext().Method
#endif

#ifndef INTERNAL_DECORATOR_COMPUTE
#define INTERNAL_DECORATOR_COMPUTE(Method) CmdList.GetComputeContext().Method
#endif

void FRHICommandBeginRenderPass::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIBeginRenderPass)(Info, Name);
}

void FRHICommandEndRenderPass::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIEndRenderPass)();
}

void FRHICommandBeginDrawingViewport::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIBeginDrawingViewport)(Viewport, RenderTargetRHI);
}

void FRHICommandEndDrawingViewport::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIEndDrawingViewport)(Viewport, bPresent, bLockToVsync);
}

void FRHICommandSetStreamSource::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetStreamSource)(StreamIndex, VertexBuffer, Offset);
}

void FRHICommandSetViewport::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetViewport)(MinX, MinY, MinZ, MaxX, MaxY, MaxZ);
}

void FRHICommandSetGraphicsPipelineState::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetGraphicsPipelineState)(PipelineState, StencilRef);
}

void FRHICommandDrawPrimitive::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIDrawPrimitive)(BaseVertexIndex, NumVertices, NumInstances);
}

void FRHICommandDrawIndexedPrimitive::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIDrawIndexedPrimitive)(BaseVertexIndex, StartIndex, NumIndices, NumInstances);
}

void FRHICommandSetStaticUniformBuffers::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR_COMPUTE(RHISetStaticUniformBuffers)(UniformBuffers);
}

void FRHICommandSetStaticUniformBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR_COMPUTE(RHISetStaticUniformBuffer)(Slot, Buffer);
}

void FRHICommandSetUniformBufferDynamicOffset::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR_COMPUTE(RHISetUniformBufferDynamicOffset)(Slot, Offset);
}