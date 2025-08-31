
#ifndef INTERNAL_DECORATOR
#define INTERNAL_DECORATOR(Method) CmdList.GetContext().Method
#endif

namespace RHI
{
void FRHICommandDrawPrimitive::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIDrawPrimitive)(BaseVertexIndex, NumPrimitives, NumInstances);
}

void FRHICommandDrawIndexedPrimitive::Execute(FRHICommandListBase& CmdList)
{
    INTERNAL_DECORATOR(RHIDrawIndexedPrimitive)(BaseVertexIndex, StartIndex, NumIndices, NumInstances);
}
}
