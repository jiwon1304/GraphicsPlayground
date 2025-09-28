#include "OpenGLDrv.h"
#include "Private/OpenGLDrvPrivate.h"

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

void FOpenGLDynamicRHI::Init()
{
    // // Create the main context (window)
    // PlatformCreateOpenGLContext(PlatformDevice, nullptr, 1280, 720, PF_B8G8R8A8, false, "Graphics Playground - OpenGL");
    // if (!PlatformContext) {
    //     assert(false && "Failed to create OpenGL context");
    //     return;
    // }

    // MainWindow = PlatformGetWindow(PlatformContext);
    // if (!MainWindow) {
    //     assert(false && "Failed to get native window handle");
    //     return;
    // }

    // // Make the context current
    // FOpenGL::MakeContextCurrent(MainWindow);

    // // Load OpenGL functions using GLAD
    // if (!FOpenGL::LoadGLLoader()) {
    //     assert(false && "Failed to load OpenGL functions");
    //     return;
    // }

    // // Set initial viewport
    // FOpenGL::Viewport(0, 0, 1280, 720);

    // // Enable V-Sync
    // glfwSwapInterval(1);

    // // Initialize any other OpenGL state here
    // InitializeStateResource();
}