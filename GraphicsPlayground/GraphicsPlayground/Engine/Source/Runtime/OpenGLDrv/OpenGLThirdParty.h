#pragma once

#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "ThirdParty/OpenGL/glad/include/KHR/khrplatform.h"
#include "ThirdParty/OpenGL/glfw/MacOS/include/GLFW/glfw3.h"
#include "ThirdParty/OpenGL/glfw/MacOS/include/GLFW/glfw3native.h"


/**
 * Choose OpenGL header by platform
 */
#ifdef _WIN32

#elif __APPLE__

#endif