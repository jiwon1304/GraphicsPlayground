#include "SlateAppMessageHandlerGLFW.h"
#include <OpenGLDrv/OpenGL3.h>

FSlateAppMessageHandlerGLFW::~FSlateAppMessageHandlerGLFW()
{
    if (Window)
        FOpenGL::SetWindowUserPointer(Window, nullptr);
}

void FSlateAppMessageHandlerGLFW::RegisterWindow(GLFWwindow *InWindow)
{
    Window = InWindow;

    FOpenGL::SetWindowUserPointer(Window, this);

    FOpenGL::SetKeyCallback(Window, &FSlateAppMessageHandlerGLFW::KeyCallback);
    FOpenGL::SetCharCallback(Window, &FSlateAppMessageHandlerGLFW::CharCallback);
    FOpenGL::SetMouseButtonCallback(Window, &FSlateAppMessageHandlerGLFW::MouseButtonCallback);
    FOpenGL::SetCursorPosCallback(Window, &FSlateAppMessageHandlerGLFW::CursorPosCallback);
    FOpenGL::SetScrollCallback(Window, &FSlateAppMessageHandlerGLFW::ScrollCallback);

    if (FOpenGL::RawMouseMotionSupported())
        FOpenGL::SetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void FSlateAppMessageHandlerGLFW::ProcessMessage()
{
    FOpenGL::PollEvents();
}

// static
FSlateAppMessageHandlerGLFW* FSlateAppMessageHandlerGLFW::GetSelf(GLFWwindow* wnd)
{
    return static_cast<FSlateAppMessageHandlerGLFW*>(glfwGetWindowUserPointer(wnd));
}

// static
void FSlateAppMessageHandlerGLFW::KeyCallback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
    auto* self = GetSelf(wnd);
    if (!self) return;

    self->UpdateModifierFromMods(mods);

    const uint32 keyCode = static_cast<uint32>(key);
    const uint32 charCode = 0; // 문자 입력은 CharCallback에서 처리
    const bool isRepeat = (action == GLFW_REPEAT);

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
        self->OnKeyDown(keyCode, charCode, isRepeat);
    else if (action == GLFW_RELEASE)
        self->OnKeyUp(keyCode, charCode, false);
}

// static
void FSlateAppMessageHandlerGLFW::CharCallback(GLFWwindow* wnd, unsigned int codepoint)
{
    auto* self = GetSelf(wnd);
    if (!self) return;

    // Windows 구현에서는 OnKeyChar 브로드캐스트를 주석 처리했으므로 동일하게 유지하거나 필요 시 활성화
    // self->OnKeyCharDelegate.Broadcast(static_cast<TCHAR>(codepoint), false);
    (void)codepoint;
}

// static
void FSlateAppMessageHandlerGLFW::MouseButtonCallback(GLFWwindow* wnd, int button, int action, int mods)
{
    auto* self = GetSelf(wnd);
    if (!self) return;

    self->UpdateModifierFromMods(mods);

    double x, y;
    glfwGetCursorPos(wnd, &x, &y);
    const FVector2D pos{static_cast<float>(x), static_cast<float>(y)};
    const auto mb = ToMouseButton(button);

    if (action == GLFW_PRESS)
    {
        // GLFW에서 더블클릭은 직접 제공되지 않음. 필요시 시간/거리 기반으로 구현.
        self->OnMouseDown(mb, pos);
    }
    else if (action == GLFW_RELEASE)
    {
        self->OnMouseUp(mb, pos);
    }
}

// static
void FSlateAppMessageHandlerGLFW::CursorPosCallback(GLFWwindow* wnd, double xpos, double ypos)
{
    auto* self = GetSelf(wnd);
    if (!self) return;

    self->UpdateCursorPosition(FVector2D{static_cast<float>(xpos), static_cast<float>(ypos)});
    self->OnMouseMove();
}

// static
void FSlateAppMessageHandlerGLFW::ScrollCallback(GLFWwindow* wnd, double xoffset, double yoffset)
{
    auto* self = GetSelf(wnd);
    if (!self) return;

    double x, y;
    glfwGetCursorPos(wnd, &x, &y);
    const FVector2D pos{static_cast<float>(x), static_cast<float>(y)};
    self->OnMouseWheel(static_cast<float>(yoffset), pos);
}

// static
EMouseButtons::Type FSlateAppMessageHandlerGLFW::ToMouseButton(int glfwButton)
{
    switch (glfwButton)
    {
    case GLFW_MOUSE_BUTTON_LEFT:   return EMouseButtons::Left;
    case GLFW_MOUSE_BUTTON_RIGHT:  return EMouseButtons::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE: return EMouseButtons::Middle;
    default:                       return EMouseButtons::Invalid;
    }
}

void FSlateAppMessageHandlerGLFW::UpdateModifierFromMods(int mods)
{
    const bool shift   = (mods & GLFW_MOD_SHIFT)   != 0;
    const bool ctrl    = (mods & GLFW_MOD_CONTROL) != 0;
    const bool alt     = (mods & GLFW_MOD_ALT)     != 0;
    const bool super   = (mods & GLFW_MOD_SUPER)   != 0; // macOS Command

    // GLFW는 좌/우 구분을 제공하지 않으므로 좌/우 모두 동일하게 반영
    ModifierKeyState[EModifierKey::LeftShift]   = shift;
    ModifierKeyState[EModifierKey::RightShift]  = shift;
    ModifierKeyState[EModifierKey::LeftControl] = ctrl;
    ModifierKeyState[EModifierKey::RightControl]= ctrl;
    ModifierKeyState[EModifierKey::LeftAlt]     = alt;
    ModifierKeyState[EModifierKey::RightAlt]    = alt;
    ModifierKeyState[EModifierKey::LeftWin]     = super;
    ModifierKeyState[EModifierKey::RightWin]    = super;

    // CapsLock은 GLFW에서 직접 제공되지 않음. 필요시 플랫폼 API 연동.
    // ModifierKeyState[EModifierKey::CapsLock] = ...
}
