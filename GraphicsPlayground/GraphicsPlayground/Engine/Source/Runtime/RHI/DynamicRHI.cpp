#include "DynamicRHI.h"
#ifdef(_WIN32)
#include "Windows/D3D11RHI/D3D11RHIPrivate.h"
#else
#include "OpenGLDrv/OpenGLDrv.h"
#endif

FDynamicRHI* GDynamicRHI = nullptr;

FDynamicRHI* PlatformCreateDynamicRHI()
{
#ifdef(_WIN32)
    return new FD3D11DynamicRHI();
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
