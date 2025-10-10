#pragma once

#include "Core/HAL/PlatformType.h"

// Windows : DirectX11, OpenGL3
// macOS  : OpenGL3
// currently BUILD_RENDER_BACKEND is defined in c_cpp_properties.json in vscode

#if defined(BUILD_PLATFORM_WINDOWS)

#if USE_OPENGL
#define BUILD_RHI_OPENGL
#elif USE_DIRECTX11
#define BUILD_RHI_DX11
#endif

#elif defined(BUILD_PLATFORM_MACOS)

#if USE_OPENGL
#define BUILD_RHI_OPENGL
#else
static_assert(false, "Only OpenGL is supported on macOS");
#endif

#else
static_assert(false, "Unsupported platform for RHI");
#endif
