// Private header. Only include in .cpp files!
#pragma once

#include "OpenGLDrv/OpenGLDrv.h"
#include "RHI/RHI.h"
#include "OpenGLDrv/OpenGLThirdParty.h"

/** 
 * Platform specific OpenGL context
 * contains viewport framebuffer and backbuffer resource/target
 */
struct FPlatformOpenGLContext;

/**
 * Platform specific OpenGL device
 * This class has to be implemented per platform and only once in one cpp file
 */
struct FPlatformOpenGLDevice;

/**
 * Initialize OpenGL on this platform. Does not create any context(=window).
 */
bool PlatformInitOpenGL();

/**
 * Create the OpenGL device.
 * This is the entrypoint to create the device instance for RHI.
 */
FPlatformOpenGLDevice* PlatformCreateOpenGLDevice();

void PlatformDestroyOpenGLDevice(FPlatformOpenGLDevice* Device);

/**
 * Create the OpenGL context.
 * Context holds most of the OpenGL things.
 */
FPlatformOpenGLContext* PlatformCreateOpenGLContext(FPlatformOpenGLDevice* Device, void* WindowHandle, 
    int32 SizeX, int32 SizeY, 
    EPixelFormat PreferredPixelFormat, bool bIsFullscreen,
    const char* WindowTitle
);

void PlatformDestroyOpenGLContext(FPlatformOpenGLContext* Context);

/**
 * Recreate the backbuffer then call this function
 */
void PlatformResizeOpenGLContext(FPlatformOpenGLDevice* Device, FPlatformOpenGLContext* Context,
    int32 SizeX, int32 SizeY, bool bIsFullscreen,
    GLenum BackBufferTarget, GLuint BackBufferResource);

/**
 * native window handle
 */
void* PlatformGetWindow(FPlatformOpenGLContext* Context);
