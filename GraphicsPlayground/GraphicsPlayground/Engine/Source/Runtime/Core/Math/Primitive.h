#pragma once
#include "Vector.h"
#include "Vector4.h"

template <typename T>
struct TPoint
{
    TPoint() : x(0), y(0) {}

    template <typename U, typename V>
    TPoint(U _x, V _y)
        : x(static_cast<T>(_x)), y(static_cast<T>(_y)) {}

    T x, y;
};

using FPoint = TPoint<float>;

template <typename T>
struct TRect
{
    TRect() : TopLeftX(0), TopLeftY(0), Width(0), Height(0) {}

    template <typename U, typename V, typename W, typename X>
    TRect(U x, V y, W w, X h)
        : TopLeftX(static_cast<T>(x)), TopLeftY(static_cast<T>(y)),
          Width(static_cast<T>(w)), Height(static_cast<T>(h)) {}

    T TopLeftX, TopLeftY, Width, Height;
};

using FRect = TRect<float>;
using IRect = TRect<int>;

template <typename T>
struct TOBB
{
    FVector4 Corners[8];
};

using FOBB = TOBB<float>;

template <typename T>
struct TBoundingBox
{
    TBoundingBox() = default;
    TBoundingBox(T InMin, T InMax) : MinLocation(InMin), MaxLocation(InMax) {}

    T MinLocation; // Minimum extents
    float pad;

    T MaxLocation; // Maximum extents
    float pad1;

    bool IsValidBox() const
    {
        return MinLocation.X <= MaxLocation.X && MinLocation.Y <= MaxLocation.Y && MinLocation.Z <= MaxLocation.Z;
    }

    static bool CheckOverlap(const TBoundingBox& A, const TBoundingBox& B)
    {
        if (A.MaxLocation.X < B.MinLocation.X || A.MinLocation.X > B.MaxLocation.X)
        {
            return false;
        }
        if (A.MaxLocation.Y < B.MinLocation.Y || A.MinLocation.Y > B.MaxLocation.Y)
        {
            return false;
        }
        if (A.MaxLocation.Z < B.MinLocation.Z || A.MinLocation.Z > B.MaxLocation.Z)
        {
            return false;
        }
        return true;
    }

    bool Intersect(const FVector& RayOrigin, const FVector& RayDir, float& OutDistance) const
    {
        float TMin = -FLT_MAX;
        float TMax = FLT_MAX;
        constexpr float epsilon = 1e-6f;

        // X축 처리
        if (FMath::Abs(RayDir.X) < epsilon)
        {
            // 레이가 X축 방향으로 거의 평행한 경우,
            // 원점의 x가 박스 [min.X, max.X] 범위 밖이면 교차 없음
            if (RayOrigin.X < MinLocation.X || RayOrigin.X > MaxLocation.X)
            {
                return false;
            }
        }
        else
        {
            float T1 = (MinLocation.X - RayOrigin.X) / RayDir.X;
            float T2 = (MaxLocation.X - RayOrigin.X) / RayDir.X;
            if (T1 > T2)
            {
                std::swap(T1, T2);
            }

            // tmin은 "현재까지의 교차 구간 중 가장 큰 min"
            TMin = (T1 > TMin) ? T1 : TMin;
            // tmax는 "현재까지의 교차 구간 중 가장 작은 max"
            TMax = (T2 < TMax) ? T2 : TMax;
            if (TMin > TMax)
            {
                return false;
            }
        }

        // Y축 처리
        if (FMath::Abs(RayDir.Y) < epsilon)
        {
            if (RayOrigin.Y < MinLocation.Y || RayOrigin.Y > MaxLocation.Y)
            {
                return false;
            }
        }
        else
        {
            float T1 = (MinLocation.Y - RayOrigin.Y) / RayDir.Y;
            float T2 = (MaxLocation.Y - RayOrigin.Y) / RayDir.Y;
            if (T1 > T2)
            {
                std::swap(T1, T2);
            }

            TMin = (T1 > TMin) ? T1 : TMin;
            TMax = (T2 < TMax) ? T2 : TMax;
            if (TMin > TMax)
            {
                return false;
            }
        }

        // Z축 처리
        if (FMath::Abs(RayDir.Z) < epsilon)
        {
            if (RayOrigin.Z < MinLocation.Z || RayOrigin.Z > MaxLocation.Z)
            {
                return false;
            }
        }
        else
        {
            float T1 = (MinLocation.Z - RayOrigin.Z) / RayDir.Z;
            float T2 = (MaxLocation.Z - RayOrigin.Z) / RayDir.Z;
            if (T1 > T2)
            {
                std::swap(T1, T2);
            }

            TMin = (T1 > TMin) ? T1 : TMin;
            TMax = (T2 < TMax) ? T2 : TMax;
            if (TMin > TMax)
            {
                return false;
            }
        }

        // 여기까지 왔으면 교차 구간 [tmin, tmax]가 유효하다.
        // tmax < 0 이면, 레이가 박스 뒤쪽에서 교차하므로 화면상 보기엔 교차 안 한다고 볼 수 있음
        if (TMax < 0.0f)
        {
            return false;
        }

        // outDistance = tmin이 0보다 크면 그게 레이가 처음으로 박스를 만나는 지점
        // 만약 tmin < 0 이면, 레이의 시작점이 박스 내부에 있다는 의미이므로, 거리를 0으로 처리해도 됨.
        OutDistance = (TMin >= 0.0f) ? TMin : 0.0f;

        return true;
    }
};

using FBoundingBox = TBoundingBox<FVector>;