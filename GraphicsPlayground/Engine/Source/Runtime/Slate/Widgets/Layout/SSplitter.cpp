#include "SSplitter.h"
#include "Launch/EngineLoop.h"

extern FEngineLoop GEngineLoop;

SSplitter::SSplitter(const FIntRect& InRect)
    : SWindow(InRect), SideLT(nullptr), SideRB(nullptr)
{
}

void SSplitter::SetSplitRatio(float InRatio)
{
    SplitRatio = FMath::Clamp(InRatio, SplitterMinRatio, 1.0f - SplitterMinRatio);
}

SSplitterH::SSplitterH(const FIntRect& InRect)
    : SSplitter(InRect)
{
    UpdateChilds();
}

uint32 SSplitterH::GetSplitValue()
{
    return Rect.GetWidth() * GetSplitRatio();
}

void SSplitterH::SetSplitValue(uint32 InValue)
{
    SetSplitRatio(static_cast<float>(InValue) / Rect.GetWidth());
}

bool SSplitterH::IsInMargin(const FIntPoint& InPoint)
{
    const uint32 SplitterCenter = GetSplitValue();
    return (
        InPoint.X >= SplitterCenter - SplitterHalfThickness
        && InPoint.X <= SplitterCenter + SplitterHalfThickness
        );
}

void SSplitterH::UpdateChilds()
{
    // 픽셀 단위로 계산하기 위해 정수형으로 변환
    const uint32 SplitterCenter = GetSplitValue();

    // Left
    if (SideLT)
    {
        SideLT->Resize(FIntRect(
            0.0f,
            0.0f,
            SplitterCenter - SplitterHalfThickness,
            Rect.GetHeight()
        ));
    }
    // Right
    if (SideRB)
    {
        SideRB->Resize(FIntRect(
            SplitterCenter + SplitterHalfThickness,
            0.0f,
            Rect.GetWidth() - (SplitterCenter + SplitterHalfThickness),
            Rect.GetHeight()
        ));
    }
}

SSplitterV::SSplitterV(const FIntRect& InRect)
    : SSplitter(InRect)
{
}

uint32 SSplitterV::GetSplitValue()
{
    return Rect.GetWidth() * GetSplitRatio();
}

void SSplitterV::SetSplitValue(uint32 InValue)
{
    SetSplitRatio(static_cast<float>(InValue) / Rect.GetWidth());
}

bool SSplitterV::IsInMargin(const FIntPoint& InPoint)
{
    const uint32 SplitterCenter = GetSplitValue();
    return (
        InPoint.Y >= SplitterCenter - SplitterHalfThickness
        && InPoint.Y <= SplitterCenter + SplitterHalfThickness
        );
}

void SSplitterV::UpdateChilds()
{
    // 픽셀 단위로 계산하기 위해 정수형으로 변환
    const uint32 SplitterCenter = GetSplitValue();

    // Left
    if (SideLT)
    {
        SideLT->Resize(FIntRect(
            0.0f,
            0.0f,
            Rect.GetWidth(),
            SplitterCenter - SplitterHalfThickness
        ));
    }
    // Right
    if (SideRB)
    {
        SideRB->Resize(FIntRect(
            0.0f,
            SplitterCenter + SplitterHalfThickness,
            Rect.GetWidth(),
            Rect.GetHeight() - (SplitterCenter + SplitterHalfThickness)
        ));
    }
}
