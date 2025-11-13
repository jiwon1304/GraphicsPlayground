#pragma once

struct GLFWwindow;

namespace FOpenGL 
{
    using Window = GLFWwindow;

    using WindowSizeCallback = void(*)(Window* window, int width, int height);
}
