#pragma once

#include "Editor/LevelEditor/SlateAppMessageHandlerBase.h"

class GLFWwindow;

class FSlateAppMessageHandlerGLFW : public FSlateAppMessageHandlerBase
{
public:
    FSlateAppMessageHandlerGLFW() = default;
    ~FSlateAppMessageHandlerGLFW();

    void RegisterWindow(GLFWwindow* InWindow);

    // GLFW uses polling
    void ProcessMessage();

private:
    static void KeyCallback(GLFWwindow* wnd, int key, int scancode, int action, int mods);
    static void CharCallback(GLFWwindow* wnd, unsigned int codepoint);
    static void MouseButtonCallback(GLFWwindow* wnd, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* wnd, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* wnd, double xoffset, double yoffset);
    static FSlateAppMessageHandlerGLFW* GetSelf(GLFWwindow* wnd);

    static EMouseButtons::Type ToMouseButton(int glfwButton);

    // GLFW mods → Modifier 상태 반영
    void UpdateModifierFromMods(int mods);

private:
    GLFWwindow* Window = nullptr;
};