#include "OpenGLDrv/Platform/OpenGLDrvPrivate.h"
#include "OpenGLDrv/OpenGLThirdParty.h"
#include "OpenGLDrv/OpenGL3.h"

#ifndef BUILD_PLATFORM_MACOS
static_assert(false, "This file is only for MacOS");
#endif

static void ErrorCallback(int Error, const char* Description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", Error, Description);
}

/**
 * Create a dummy window to initialize OpenGL context
 * This is needed to load OpenGL functions using GLAD
 */
static void PlatformCreateDummyOpenGLWindow(FPlatformOpenGLContext* OutContext)
{
    OutContext->Window = FOpenGL::CreateWindowGLFW(1, 1, "Dummy");
    FOpenGL::MakeContextCurrent(OutContext->Window);
    FOpenGL::LoadGLLoader();
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
     // For initialization, it does not represent any real window or rendering context
    FPlatformOpenGLContext DummyContext;
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

    PlatformCreateDummyOpenGLWindow(&Device->DummyContext);

    return Device;
}

void PlatformDestroyOpenGLDevice(FPlatformOpenGLDevice* Device)
{
    assert(Device);
    assert(Device->MainContext.Window == nullptr); // Main context should be destroyed before device

    PlatformDestroyOpenGLContext(&Device->DummyContext);
    Device->DummyContext.Window = nullptr;

    // PlatformDestroyOpenGLContext(&Device->MainContext);
    // Device->MainContext.Window = nullptr;

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
}

void PlatformDestroyOpenGLContext(FPlatformOpenGLContext* Context)
{
    FOpenGL::DestroyWindow(Context->Window);
    Context->Window = nullptr;
}

void PlatformResizeOpenGLContext(FPlatformOpenGLContext* Context,
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