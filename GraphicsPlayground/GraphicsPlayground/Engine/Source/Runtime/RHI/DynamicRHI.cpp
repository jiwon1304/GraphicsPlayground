#include "DynamicRHI.h"

RHI::FDynamicRHI* GDynamicRHI = nullptr;

void RHIInit()
{
    assert(GDynamicRHI == nullptr);
    GDynamicRHI = PlatformCreateDynamicRHI();
}