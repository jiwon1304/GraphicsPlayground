#pragma once
#include "Core/HAL/PlatformType.h"
#include "ThirdParty/imgui/imgui.h"

class FImGuiManager
{
public:
    // void Initialize(HWND hWnd, FGraphicsDevice* InGraphics);
    virtual void BeginFrame() const = 0;
    virtual void EndFrame() const = 0;
    ImGuiContext* GetContext() const;
    virtual void Shutdown() = 0;
    
    static void ApplySharedStyle(ImGuiContext* Context1, ImGuiContext* Context2);
    
protected:
    ImGuiContext* ImGuiContext = nullptr;
    ImFont* SharedFont;
    
    void InitializeStyle();

private:
    void PreferenceStyle() const;
};

