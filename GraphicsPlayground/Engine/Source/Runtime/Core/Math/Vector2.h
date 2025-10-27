#pragma once
#include <cassert>
#include <compare>
#include "MathUtility.h"
#include "Misc/Parse.h"
#include "MathFwd.h"

template <typename T>
struct TVector2
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
    T X, Y;

    using FReal = T;

    constexpr TVector2() : X(0), Y(0) {}
    constexpr TVector2(T InX, T InY) : X(InX), Y(InY) {}
    constexpr TVector2(T Scalar) : X(Scalar), Y(Scalar) {}
    constexpr TVector2(const TVector2&) = default;
    constexpr TVector2& operator=(const TVector2&) = default;

    static const TVector2<T> ZeroVector;
    static const TVector2<T> OneVector;

    constexpr TVector2 operator+(const TVector2& Rhs) const
    {
        return {
            X + Rhs.X,
            Y + Rhs.Y
        };
    }

    constexpr TVector2 operator-(const TVector2& Rhs) const
    {
        return {
            X - Rhs.X,
            Y - Rhs.Y
        };
    }

    constexpr TVector2 operator*(T Scalar) const
    {
        return {
            X * Scalar,
            Y * Scalar
        };
    }

    constexpr TVector2 operator/(T Scalar) const
    {
        return {
            X / Scalar,
            Y / Scalar
        };
    }

    constexpr TVector2& operator+=(const TVector2& Rhs)
    {
        X += Rhs.X;
        Y += Rhs.Y;
        return *this;
    }

    constexpr bool operator==(const TVector2& Vector2D) const = default;
    constexpr bool operator!=(const TVector2& Vector2D) const = default;

    FString ToString() const
    {
        return FString::Printf(TEXT("X=%3.3f Y=%3.3f"), X, Y);
    }

    bool InitFromString(const FString& InSourceString)
    {
        X = Y = 0;

        // The initialization is only successful if the X and Y values can all be parsed from the string
        const bool bSuccessful = FParse::Value(*InSourceString, TEXT("X=") , X) && FParse::Value(*InSourceString, TEXT("Y="), Y) ;

        return bSuccessful;
    }

    [[nodiscard]] constexpr T SquaredLength() const { return X * X + Y * Y; }
    [[nodiscard]] constexpr T SizeSquared() const { return SquaredLength(); }
    [[nodiscard]] constexpr T Length() const { return FMath::Sqrt(SquaredLength()); }
    [[nodiscard]] constexpr T Size() const { return Length(); }
    [[nodiscard]] constexpr bool Equals(const TVector2& Other, T Tolerance = KINDA_SMALL_NUMBER) const
    {
        return FMath::Abs(X - Other.X) <= Tolerance && FMath::Abs(Y - Other.Y) <= Tolerance;
    }
};

template <typename T>
const TVector2<T> TVector2<T>::ZeroVector = TVector2<T>(0);

template <typename T>
const TVector2<T> TVector2<T>::OneVector = TVector2<T>(1);

template <typename T>
FArchive& operator<<(FArchive& Ar, TVector2<T>& V)
{
    return Ar << V.X << V.Y;
}
