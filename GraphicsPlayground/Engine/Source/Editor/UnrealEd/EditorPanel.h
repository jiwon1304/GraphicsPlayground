#pragma once
#include <World/WorldType.h>

#ifndef __ICON_FONT_INDEX__

#define __ICON_FONT_INDEX__
#define DEFAULT_FONT        0
#define    FEATHER_FONT        1

#endif // !__ICON_FONT_INDEX__

class FGenericWindow;

enum EWindowType : uint8
{
    WT_Main,
    WT_ParticleSubWindow
};

class UEditorPanel
{
public:
    UEditorPanel() : Width(0), Height(0) {}
    UEditorPanel(float InWidth, float InHeight) : Width(InWidth), Height(InHeight) {}
    virtual ~UEditorPanel() = default;
    virtual void Render() = 0;
    void OnResize(const FGenericWindow* Window);

    void  SetSupportedWorldTypes(EWorldTypeBitFlag mask) { SupportedMask = mask; }
    EWorldTypeBitFlag GetSupportedWorldTypes() const { return SupportedMask; }

    EWindowType WindowType = WT_Main;

protected:
    float Width;
    float Height;
private:
    EWorldTypeBitFlag SupportedMask = EWorldTypeBitFlag::None;
};
