#pragma once

#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "ThirdParty/OpenGL/glad/include/KHR/khrplatform.h"


/**
 * Choose OpenGL header by platform
 */
#ifdef _WIN32
#include "ThirdParty/OpenGL/glfw/WIN64/include/GLFW/glfw3.h"
#include "ThirdParty/OpenGL/glfw/WIN64/include/GLFW/glfw3native.h"

#elif __APPLE__
#include "ThirdParty/OpenGL/glfw/MACOS/include/GLFW/glfw3.h"
#include "ThirdParty/OpenGL/glfw/MACOS/include/GLFW/glfw3native.h"

#else
static_assert(false, "Unsupported platform for OpenGL");

#endif