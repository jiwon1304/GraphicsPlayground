#include "OpenGLDrv.h"

void* FOpenGLDynamicRHI::RHIGetNativeDevice()
{
    return PlatformDevice;
}
