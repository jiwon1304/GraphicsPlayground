#include "SWindow.h"

SWindow::SWindow(FRect InRect)
    : Rect(InRect)
{}

void SWindow::Initialize(FRect InitRect)
{
    Rect = InitRect;
}

void SWindow::OnResize(uint32 InWidth, uint32 InHeight)
{
    Rect.SetWidth(InWidth);
    Rect.SetHeight(InHeight);
}

bool SWindow::IsHover(const FPoint& InPoint) 
{
    bIsHovered = Rect.Contains(InPoint);
    return bIsHovered;
}

bool SWindow::OnPressed(const FPoint& InPoint)
{
    return false;
}

bool SWindow::OnReleased() 
{
    return false;
}
