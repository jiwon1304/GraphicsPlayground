#ifndef INTERNAL_DECORATOR
#define INTERNAL_DECORATOR(Method) CmdList.GetContext().Method
#endif

namespace RHI
{

// ----- Render Pass -----
void FRHICommandBeginRenderPass::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIBeginRenderPass)(Info, Name);
}

void FRHICommandEndRenderPass::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIEndRenderPass)();
}

// ----- Input Assembly -----
void FRHICommandSetPrimitiveTopology::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetPrimitiveTopology)(PrimitiveType);
}

void FRHICommandSetInputLayout::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetInputLayout)(InputLayout);
}

void FRHICommandSetVertexBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetVertexBuffer)(Slot, VertexBuffer, Stride, Offset);
}

void FRHICommandSetIndexBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetIndexBuffer)(Slot, IndexBuffer, Offset);
}

// ----- Shaders -----
void FRHICommandSetVertexShader::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetVertexShader)(Shader);
}

void FRHICommandSetPixelShader::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetPixelShader)(Shader);
}

void FRHICommandSetComputeShader::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetComputeShader)(Shader);
}

void FRHICommandSetGeometryShader::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetGeometryShader)(Shader);
}

// ----- Shader Resources / Uniforms -----
void FRHICommandSetStaticUniformBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetStaticUniformBuffer)(TargetShader, Slot, UniformBuffer);
}

void FRHICommandSetDynamicUniformBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetDynamicUniformBuffer)(TargetShader, Slot, UniformBuffer);
}

void FRHICommandSetShaderResourceView::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetShaderResourceView)(TargetShader, Slot, SRV);
}

void FRHICommandSetSampler::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetSampler)(TargetShader, Slot, SamplerState);
}

// ----- Pipeline / Fixed States -----
void FRHICommandSetRasterizerState::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetRasterizerState)(State);
}

void FRHICommandSetBlendState::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetBlendState)(State, BlendFactor, SampleMask);
}

void FRHICommandSetDepthStencilState::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetDepthStencilState)(State, StencilRef);
}

void FRHICommandSetViewport::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHISetViewport)(Viewport);
}

// ----- Output Merger -----
void FRHICommandSetRenderTargets::Execute(FRHICommandListBase& CmdList)
{
    // RTVs is a TArray<FRHIView*> we stored at record time.
    INTERNAL_DECORATOR(RHISetRenderTargets)(NumRTVs, RTVs.GetData(), DSV);
}

// ----- Updates -----
void FRHICommandUpdateBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIUpdateBuffer)(Buffer, Copy, Size);
}

void FRHICommandUpdateUniformBuffer::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIUpdateUniformBuffer)(UniformBuffer, Copy, Size);
}

void FRHICommandUpdateTexture::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIUpdateTexture)(Texture, Copy, Size);
}

void FRHICommandUpdateViewport::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIUpdateViewport)(Viewport, Desc);
}

// ----- Clear -----
void FRHICommandClearColorTexture::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIClearColorTexture)(Texture, ClearColor);
}

void FRHICommandClearDepthTexture::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIClearDepthTexture)(Texture, Depth, Stencil);
}

// ----- Draw -----
void FRHICommandDrawPrimitive::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIDrawPrimitive)(BaseVertexIndex, NumVertices, NumInstances);
}

void FRHICommandDrawIndexedPrimitive::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIDrawIndexedPrimitive)(BaseVertexIndex, StartIndex, NumIndices, NumInstances);
}

} // namespace RHI