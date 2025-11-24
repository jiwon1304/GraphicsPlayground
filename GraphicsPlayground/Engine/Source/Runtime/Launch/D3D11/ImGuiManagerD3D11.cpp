#include "ImGuiManagerD3D11.h"
#include "ThirdParty/imgui/backends/imgui_impl_dx11.h"
#include "ThirdParty/imgui/backends/imgui_impl_win32.h"

void FImGuiManagerD3D11::Initialize(HWND hWnd, ID3D11Device *InDevice, ID3D11DeviceContext *InDeviceContext)
{
    IMGUI_CHECKVERSION();
    ImGuiContext = ImGui::CreateContext();
    ImGuiIO& IO = ImGui::GetIO();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(InDevice, InDeviceContext);
    InitializeStyle();
}

void FImGuiManagerD3D11::BeginFrame() const
{
    ImGui::SetCurrentContext(ImGuiContext);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void FImGuiManagerD3D11::EndFrame() const
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void FImGuiManagerD3D11::Shutdown()
{
    ImGui::SetCurrentContext(ImGuiContext);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(ImGuiContext);
}
