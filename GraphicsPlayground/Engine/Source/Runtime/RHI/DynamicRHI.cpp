#include "DynamicRHI.h"
#include "RHIResources.h"
#ifdef BUILD_PLATFORM_WINDOWS
#include "Windows/D3D11RHI/D3D11RHIPrivate.h"
#else
#include "OpenGLDrv/OpenGLDrv.h"
#endif

FDynamicRHI* GDynamicRHI = nullptr;

FDynamicRHI* PlatformCreateDynamicRHI()
{
#ifdef BUILD_PLATFORM_WINDOWS
    // return new FD3D11DynamicRHI();
    return nullptr;
#else
    return new FOpenGLDynamicRHI();
#endif
}

// EngineLoop 초기화 할 때 호출
void RHIInit()
{
    assert(GDynamicRHI == nullptr);
    GDynamicRHI = PlatformCreateDynamicRHI();
}

FGraphicsPipelineStateRHIRef FDynamicRHIPSOFallback::RHICreateGraphicsPipelineState(const FGraphicsPipelineStateInitializer &Initializer)
{
    return new FRHIGraphicsPipelineStateFallBack(Initializer);
}
