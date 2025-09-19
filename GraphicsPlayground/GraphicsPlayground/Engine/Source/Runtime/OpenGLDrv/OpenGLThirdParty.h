#pragma once

#include "Core/HAL/PlatformType.h"
#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "ThirdParty/OpenGL/glad/include/KHR/khrplatform.h"

/**
 * Choose OpenGL header by platform
 */
#if defined(BUILD_PLATFORM_WINDOWS)
#include "ThirdParty/OpenGL/glfw/WIN64/include/GLFW/glfw3.h"
#include "ThirdParty/OpenGL/glfw/WIN64/include/GLFW/glfw3native.h"

#elif defined(BUILD_PLATFORM_MAC)
#include "ThirdParty/OpenGL/glfw/MACOS/include/GLFW/glfw3.h"
#include "ThirdParty/OpenGL/glfw/MACOS/include/GLFW/glfw3native.h"

#else
static_assert(false, "Unsupported platform for OpenGL");
#endif