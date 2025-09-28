#include "OpenGLDrv/Private/OpenGLDrvPrivate.h"
#include "OpenGLDrv/OpenGLThirdParty.h"
#include "OpenGLDrv/OpenGL3.h"

#ifndef BUILD_PLATFORM_MAC
static_assert(false, "This file is only for MacOS");
#endif

static void ErrorCallback(int Error, const char* Description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", Error, Description);
}

// ???
static void framebufferSizeCallback(FOpenGL::Window* /*win*/, int width, int height) {
    if (width > 0 && height > 0) {
        FOpenGL::Viewport(0, 0, width, height);
    }
}

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

struct FPlatformOpenGLDevice
{
    FPlatformOpenGLContext MainContext;
};

bool PlatformInitOpenGL()
{
    FOpenGL::SetErrorCallback(ErrorCallback);

    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // macOS 필수

    // HiDPI(레티나)에서 content scale 적용 예시
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
}

FPlatformOpenGLDevice* PlatformCreateOpenGLDevice()
{
    FPlatformOpenGLDevice* Device = static_cast<FPlatformOpenGLDevice*>(FPlatformMemory::Malloc<EAT_RHI>(sizeof(FPlatformOpenGLDevice)));
    if (!Device) {
        std::fprintf(stderr, "Failed to allocate memory for OpenGL device\n");
        return nullptr;
    }

    return Device;
}

void PlatformDestroyOpenGLDevice(FPlatformOpenGLDevice* Device)
{
    assert(Device);

    assert(Device->MainContext.Window == nullptr);
    FPlatformMemory::Free<EAT_RHI>(Device, sizeof(FPlatformOpenGLDevice));
    FOpenGL::Terminate();
}

FPlatformOpenGLContext* PlatformCreateOpenGLContext(FPlatformOpenGLDevice* Device, void* WindowHandle, 
    int32 SizeX, int32 SizeY, 
    EPixelFormat PreferredPixelFormat, bool bIsFullscreen,
    const char* WindowTitle)
{
    Device->MainContext.Window = FOpenGL::CreateWindowGLFW(SizeX, SizeY, WindowTitle);
    if (!Device->MainContext.Window) {
        std::fprintf(stderr, "Failed to create OpenGL window\n");
        return nullptr;
    }

    FOpenGL::MakeContextCurrent(Device->MainContext.Window);
    
    if(!FOpenGL::LoadGLLoader()) {
        std::fprintf(stderr, "Failed to initialize OpenGL context\n");
        FOpenGL::DestroyWindow(Device->MainContext.Window);
        Device->MainContext.Window = nullptr;
        return nullptr;
    }
}

void PlatformDestroyOpenGLContext(FPlatformOpenGLContext* Context)
{
    FOpenGL::DestroyWindow(Context->Window);
    Context->Window = nullptr;
}

void PlatformResizeOpenGLContext(FPlatformOpenGLDevice* Device, FPlatformOpenGLContext* Context,
    int32 SizeX, int32 SizeY, bool bIsFullscreen,
    GLenum BackBufferTarget, GLuint BackBufferResource)
{
    if (Context && Context->Window && SizeX > 0 && SizeY > 0)
    {
        FOpenGL::Viewport(0, 0, SizeX, SizeY);
        Context->BackBufferTarget = BackBufferTarget;
        Context->BackBufferResource = BackBufferResource;
    }
}

// MacOS has no native window handle
void* PlatformGetWindow(FPlatformOpenGLContext* Context)
{
    return (void*)Context->Window;
}