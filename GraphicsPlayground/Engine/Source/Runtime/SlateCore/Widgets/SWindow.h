#pragma once

#include "Launch/Define.h"
#include "Math/Rect.h"

/**
 * A simple class for window size modification
 */
class SWindow
{
public:
    SWindow(const FIntRect& InRect);

    virtual void Resize(const FIntRect& InRect) { Rect = InRect; }

    FIntRect GetRect() const { return Rect; }
    
    virtual bool Contains(const FIntPoint& InPoint);
    
protected:
    FIntRect Rect;
};

