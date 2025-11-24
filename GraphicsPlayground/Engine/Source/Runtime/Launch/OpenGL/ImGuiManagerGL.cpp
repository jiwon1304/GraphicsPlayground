#include "ImGuiManagerGL.h"
#include "ThirdParty/imgui/backends/imgui_impl_glfw.h"
#include "ThirdParty/imgui/backends/imgui_impl_opengl3.h"

void FImGuiManagerOpenGL::Initialize(FOpenGL::Window *InWindow)
{
    IMGUI_CHECKVERSION();
    ImGuiContext = ImGui::CreateContext();
    ImGuiIO& IO = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(InWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void FImGuiManagerOpenGL::BeginFrame() const
{
    ImGui::SetCurrentContext(ImGuiContext);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void FImGuiManagerOpenGL::EndFrame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void FImGuiManagerOpenGL::Shutdown()
{
    ImGui::SetCurrentContext(ImGuiContext);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(ImGuiContext);
}
