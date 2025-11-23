#include "SWindow.h"

SWindow::SWindow(const FIntRect& InRect)
    : Rect(InRect)
{}

bool SWindow::Contains(const FIntPoint& InPoint) 
{
    return Rect.Contains(InPoint);
}
