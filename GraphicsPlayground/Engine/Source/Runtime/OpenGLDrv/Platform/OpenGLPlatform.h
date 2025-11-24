#pragma once

#include "OpenGLDrv/OpenGLThirdParty.h"
#include "OpenGLDrv/OpenGLFwd.h"
#include "RHI/PixelFormat.h"

/**
 * OpenGL does not have explicit context.
 * Rather, the context is implicitly bound to the current window.
 */
struct FPlatformOpenGLContext
{
    FOpenGL::Window* Window;
    GLuint ViewportFramebuffer;
    GLuint BackBufferResource;
    GLenum BackBufferTarget;
};

// We currently use one window
struct FPlatformOpenGLDevice
{
    FPlatformOpenGLContext MainContext;
     // For initialization, it does not represent any real window or rendering context
    FPlatformOpenGLContext DummyContext;
};

extern FPlatformOpenGLDevice* GOpenGLDevice;

/**
 * These should be called by the RHI thread only.
 */

extern bool PlatformInitOpenGL();

extern FPlatformOpenGLDevice* PlatformCreateOpenGLDevice();

extern void PlatformDestroyOpenGLDevice(FPlatformOpenGLDevice* Device);

extern FPlatformOpenGLContext* PlatformCreateOpenGLContext(FPlatformOpenGLDevice* Device, void* WindowHandle,
    int32 SizeX, int32 SizeY,
    EPixelFormat PreferredPixelFormat, bool bIsFullscreen,
    const char* WindowTitle);

extern void PlatformDestroyOpenGLContext(FPlatformOpenGLContext* Context);

extern void PlatformResizeOpenGLContext(FPlatformOpenGLContext* Context,
    int32 SizeX, int32 SizeY, bool bIsFullscreen,
    GLenum BackBufferTarget, GLuint BackBufferResource);
