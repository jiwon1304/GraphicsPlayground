#pragma once
#include "Core/HAL/PlatformType.h"
#include "ThirdParty/ImGui/imgui.h"

class FGraphicsDevice;

class UImGuiManager
{
public:
    void Initialize(HWND hWnd, FGraphicsDevice* InGraphics);
    void BeginFrame() const;
    void EndFrame() const;
    void PreferenceStyle() const;
    ImGuiContext* GetContext() const;
    void Shutdown();

    static void ApplySharedStyle(ImGuiContext* Context1, ImGuiContext* Context2);
    ImFont* SharedFont;
private:
    ImGuiContext* ImGuiContext = nullptr;
};

