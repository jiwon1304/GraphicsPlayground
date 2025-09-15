#pragma once

#include "Core/HAL/PlatformType.h"

template <typename T>
class TPoint
{
public:
    TPoint() : X(0), Y(0) {}
    TPoint(T InX, T InY) : X(InX), Y(InY) {}

    bool operator==(const TPoint& Other) const
    {
        return X == Other.X && Y == Other.Y;
    }

    bool operator!=(const TPoint& Other) const
    {
        return !(*this == Other);
    }

    T X;
    T Y;
};

template <typename T>
class TRect
{
public:
    TRect() : Min(T(0), T(0)), Max(T(0), T(0)) {}
    TRect(T InMinX, T InMinY, T InMaxX, T InMaxY)
        : Min(InMinX, InMinY), Max(InMaxX, InMaxY) {}
    TRect(const TPoint<T>& InMin, const TPoint<T>& InMax)
        : Min(InMin), Max(InMax) {}

    TPoint<T> Min;
    TPoint<T> Max;
};

using FIntPoint = TPoint<int32>;
using FIntRect = TRect<int32>;
using FFloatPoint = TPoint<float>;
using FFloatRect = TRect<float>;