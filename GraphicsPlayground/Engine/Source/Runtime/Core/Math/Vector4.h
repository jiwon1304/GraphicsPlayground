#pragma once
#include "Vector.h"

// 4D Vector (template)
template <typename T>
struct TVector4
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
    T X, Y, Z, W;

    using FReal = T;

public:
    constexpr TVector4() : X(0), Y(0), Z(0), W(0) {}
    constexpr TVector4(T InX, T InY, T InZ, T InW)
        : X(InX), Y(InY), Z(InZ), W(InW)
    {}
    constexpr TVector4(const TVector<T>& InVector, T InW = 0)
        : X(InVector.X), Y(InVector.Y), Z(InVector.Z), W(InW)
    {}
    TVector4(const FString& SourceString)
        : X(0), Y(0), Z(0), W(0)
    {
        InitFromString(SourceString);
    }

    constexpr TVector4<T> operator+(const TVector4<T>& Other) const
    {
        return { X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W };
    }
    constexpr TVector4<T> operator-(const TVector4<T>& Other) const
    {
        return { X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W };
    }
    constexpr TVector4<T> operator/(T Scalar) const
    {
        return { X / Scalar, Y / Scalar, Z / Scalar, W / Scalar };
    }

    constexpr bool operator==(const TVector4<T>& Other) const = default;
    constexpr bool operator!=(const TVector4<T>& Other) const = default;

    [[nodiscard]] constexpr bool Equals(const TVector4<T>& Other, T Tolerance = KINDA_SMALL_NUMBER) const
    {
        return FMath::Abs(X - Other.X) <= Tolerance
            && FMath::Abs(Y - Other.Y) <= Tolerance
            && FMath::Abs(Z - Other.Z) <= Tolerance
            && FMath::Abs(W - Other.W) <= Tolerance;
    }

    FString ToString() const
    {
        // FString::Printf를 사용하여 포맷팅된 문자열 생성
        // TEXT() 매크로는 리터럴 문자열을 TCHAR 타입으로 만들어줍니다.
        return FString::Printf(TEXT("X=%3.3f Y=%3.3f Z=%3.3f W=%3.3f"), X, Y, Z, W);

        // 필요에 따라 소수점 정밀도 지정 가능: 예) "X=%.2f Y=%.2f Z=%.2f"
        // return FString::Printf(TEXT("X=%.2f Y=%.2f Z=%.2f"), x, y, z);
    }

    bool InitFromString(const FString& InSourceString)
    {
        X = Y = Z = 0;
        W = 1.0;

        // The initialization is only successful if the X, Y, and Z values can all be parsed from the string
        const bool bSuccessful = FParse::Value(*InSourceString, TEXT("X=") , X) &&
                FParse::Value(*InSourceString, TEXT("Y="), Y)   &&
                FParse::Value(*InSourceString, TEXT("Z="), Z);

        // W is optional, so don't factor in its presence (or lack thereof) in determining initialization success
        FParse::Value(*InSourceString, TEXT("W="), W);

        return bSuccessful;
    }

    constexpr TVector<T> xyz() const
    {
        return { X, Y, Z };
    }
};

template <typename T>
inline FArchive& operator<<(FArchive& Ar, TVector4<T>& V)
{
    return Ar << V.X << V.Y << V.Z << V.W;
}
