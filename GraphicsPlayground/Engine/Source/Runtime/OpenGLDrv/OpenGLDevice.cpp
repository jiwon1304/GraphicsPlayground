#include "OpenGLDrv.h"
#include "Platform/OpenGLDrvPrivate.h"

FOpenGLDynamicRHI::FOpenGLDynamicRHI()
{
    assert(Singleton == nullptr);
    Singleton = this;

    // Initialize platform specific OpenGL
    if (!PlatformInitOpenGL()) {
        assert(false && "Failed to initialize OpenGL");
        return;
    }

    // Create the device
    PlatformDevice = PlatformCreateOpenGLDevice();
    if (!PlatformDevice) {
        assert(false && "Failed to create OpenGL device");
        return;
    }
}

FOpenGLDynamicRHI::~FOpenGLDynamicRHI()
{
}

void FOpenGLDynamicRHI::Init()
{

}

void FOpenGLDynamicRHI::Shutdown()
{
}

void FOpenGLDynamicRHI::RHIEndFrame_RenderThread(FRHICommandListImmediate &RHICmdList)
{
}

void FOpenGLDynamicRHI::RHIEndFrame(uint64 FrameNumber)
{
}
