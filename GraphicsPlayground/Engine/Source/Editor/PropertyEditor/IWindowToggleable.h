#pragma once
#include "Core/HAL/PlatformType.h"

class FGenericWindow;

class IWindowToggleable
{
public:
    virtual void Toggle() = 0;
    virtual ~IWindowToggleable() = default;

    void Resize(uint32 InWidth, uint32 InHeight) {
        Width = InWidth;
        Height = InHeight;
    }
    
protected:
    uint32 Width;
    uint32 Height;
};
