#pragma once
#include <cassert>
#include <compare>
#include "MathUtility.h"
#include "MathFwd.h"
#include "Serialization/Archive.h"
#include "Misc/Parse.h"
#include "Rotator.h"

template <typename T>
struct TVector
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
    alignas(16) T X, Y, Z;

    using FReal = T;

public:
    constexpr TVector() : X(0), Y(0), Z(0) {}
    constexpr TVector(T X, T Y, T Z) : X(X), Y(Y), Z(Z) {}
    constexpr explicit TVector(T Scalar) : X(Scalar), Y(Scalar), Z(Scalar) {}
    explicit TVector(const TVector4<T>& V);
    constexpr TVector(const TVector&) = default;
    constexpr TVector& operator=(const TVector&) = default;

    static const TVector<T> ZeroVector;
    static const TVector<T> OneVector;
    static const TVector<T> UpVector;
    static const TVector<T> DownVector;
    static const TVector<T> ForwardVector;
    static const TVector<T> BackwardVector;
    static const TVector<T> RightVector;
    static const TVector<T> LeftVector;
    static const TVector<T> XAxisVector;
    static const TVector<T> YAxisVector;
    static const TVector<T> ZAxisVector;

public:
    static constexpr TVector<T> Zero() { return ZeroVector; }
    static constexpr TVector<T> One() { return OneVector; }
    static constexpr TVector<T> UnitX() { return XAxisVector; }
    static constexpr TVector<T> UnitY() { return YAxisVector; }
    static constexpr TVector<T> UnitZ() { return ZAxisVector; }

    static constexpr T DistSquared(const TVector<T>& V1, const TVector<T>& V2)
    {
        return (V2.X-V1.X)*(V2.X-V1.X) + (V2.Y-V1.Y)*(V2.Y-V1.Y) + (V2.Z-V1.Z)*(V2.Z-V1.Z);
    }

    static constexpr T Distance(const TVector<T>& V1, const TVector<T>& V2)
    {
        return FMath::Sqrt(DistSquared(V1, V2));
    }
    static constexpr T Dist(const TVector<T>& V1, const TVector<T>& V2) { return Distance(V1, V2); }

    static constexpr T DistSquaredXY(const TVector<T>& V1, const TVector<T>& V2)
    {
        return (V2.X-V1.X)*(V2.X-V1.X) + (V2.Y-V1.Y)*(V2.Y-V1.Y);
    }
    static constexpr T DistSquared2D(const TVector<T>& V1, const TVector<T>& V2) { return DistSquaredXY(V1, V2); }

    static constexpr T DistXY(const TVector<T>& V1, const TVector<T>& V2)
    {
        return FMath::Sqrt(DistSquaredXY(V1, V2));
    }
    static constexpr T Dist2D(const TVector<T>& V1, const TVector<T>& V2) { return DistXY(V1, V2); }

    constexpr T operator|(const TVector<T>& Other) const { return X * Other.X + Y * Other.Y + Z * Other.Z; }
    constexpr T Dot(const TVector<T>& Other) const { return *this | Other; }
    static constexpr T DotProduct(const TVector<T>& A, const TVector<T>& B) { return A | B; }

    constexpr TVector<T> operator^(const TVector<T>& Other) const
    {
        return TVector<T>{
            Y * Other.Z - Z * Other.Y,
            Z * Other.X - X * Other.Z,
            X * Other.Y - Y * Other.X
        };
    }
    constexpr TVector<T> Cross(const TVector<T>& Other) const { return *this ^ Other; }
    static constexpr TVector<T> CrossProduct(const TVector<T>& A, const TVector<T>& B) { return A ^ B; }

    constexpr TVector<T> operator+(const TVector<T>& Other) const { return {X + Other.X, Y + Other.Y, Z + Other.Z}; }
    constexpr TVector<T> operator+(T Scalar) const { return {X + Scalar, Y + Scalar, Z + Scalar}; }
    constexpr TVector<T>& operator+=(const TVector<T>& Other) { X += Other.X; Y += Other.Y; Z += Other.Z; return *this; }

    constexpr TVector<T> operator-(const TVector<T>& Other) const { return {X - Other.X, Y - Other.Y, Z - Other.Z}; }
    constexpr TVector<T> operator-(T Scalar) const { return {X - Scalar, Y - Scalar, Z - Scalar}; }
    constexpr TVector<T>& operator-=(const TVector<T>& Other) { X -= Other.X; Y -= Other.Y; Z -= Other.Z; return *this; }

    constexpr TVector<T> operator*(const TVector<T>& Other) const { return {X * Other.X, Y * Other.Y, Z * Other.Z}; }
    constexpr TVector<T> operator*(T Scalar) const { return {X * Scalar, Y * Scalar, Z * Scalar}; }
    constexpr TVector<T>& operator*=(const TVector<T>& Other) { X *= Other.X; Y *= Other.Y; Z *= Other.Z; return *this; }
    constexpr TVector<T>& operator*=(T Scalar) { X *= Scalar; Y *= Scalar; Z *= Scalar; return *this; }

    constexpr TVector<T> operator/(const TVector<T>& Other) const { return {X / Other.X, Y / Other.Y, Z / Other.Z}; }
    constexpr TVector<T> operator/(T Scalar) const { return {X / Scalar, Y / Scalar, Z / Scalar}; }
    constexpr TVector<T>& operator/=(T Scalar) { X /= Scalar; Y /= Scalar; Z /= Scalar; return *this; }

    constexpr TVector<T> operator-() const { return {-X, -Y, -Z}; }

    constexpr bool operator==(const TVector<T>& Other) const { return X == Other.X && Y == Other.Y && Z == Other.Z; }
    constexpr bool operator!=(const TVector<T>& Other) const { return X != Other.X || Y != Other.Y || Z != Other.Z; }

    constexpr std::partial_ordering operator<=>(const TVector<T>& Other) const { return SizeSquared() <=> Other.SizeSquared(); }
    constexpr std::partial_ordering operator<=>(T Scalar) const { return SizeSquared() <=> Scalar * Scalar; }
    constexpr std::partial_ordering operator<=>(float Scalar, const TVector<T>& Vector) const { return Scalar * Scalar <=> Vector.SizeSquared();}

    constexpr T& operator[](size_t Index)
    {
        assert(Index < 3);
        return reinterpret_cast<T*>(this)[Index];
    }

    constexpr const T& operator[](size_t Index) const
    {
        assert(Index < 3);
        return reinterpret_cast<const T*>(this)[Index];
    }

    bool ContainsNaN() const
    {
        return (!FMath::IsFinite(X) || 
                !FMath::IsFinite(Y) ||
                !FMath::IsFinite(Z));
    }

    constexpr T SquaredLength() const { return X * X + Y * Y + Z * Z; }
    constexpr T SizeSquared() const { return SquaredLength(); }
    constexpr T Length() const { return FMath::Sqrt(SquaredLength()); }
    constexpr T Size() const { return Length(); }

    constexpr bool Equals(const TVector<T>& V, T Tolerance = KINDA_SMALL_NUMBER) const
    {
        return FMath::Abs(X-V.X) <= Tolerance && FMath::Abs(Y-V.Y) <= Tolerance && FMath::Abs(Z-V.Z) <= Tolerance;
    }

    constexpr bool AllComponentsEqual(T Tolerance = KINDA_SMALL_NUMBER) const
    {
        return FMath::Abs(X - Y) <= Tolerance && FMath::Abs(X - Z) <= Tolerance && FMath::Abs(Y - Z) <= Tolerance;
    }

    constexpr T GetMin() const { return FMath::Min(FMath::Min(X, Y), Z); }
    constexpr T GetMax() const { return FMath::Max(FMath::Max(X, Y), Z); }

    constexpr bool Normalize(T Tolerance)
    {
        const float SquareSum = X * X + Y * Y + Z * Z;
        if (SquareSum > Tolerance)
        {
            const float Scale = FMath::InvSqrt(SquareSum);
            X *= Scale; Y *= Scale; Z *= Scale;
            return true;
        }
        return false;
    }

    constexpr inline bool IsNormalized() const
    {
        constexpr T ThreshVectorNormalized = KINDA_SMALL_NUMBER;
        return (FMath::Abs(1.f - SizeSquared()) < ThreshVectorNormalized);
    }

    constexpr TVector<T> GetUnsafeNormal() const
    {
        const float Scale = FMath::InvSqrt(X*X + Y*Y + Z*Z);
        return {X * Scale, Y * Scale, Z * Scale};
    }

    constexpr TVector<T> GetSafeNormal(T Tolerance = KINDA_SMALL_NUMBER) const
    {
        constexpr float SquareSum = X*X + Y*Y + Z*Z;

        // Not sure if it's safe to add tolerance in there. Might introduce too many errors
        if (SquareSum == 1.f)
        {
            return *this;
        }
        else if (SquareSum < Tolerance)
        {
            return ZeroVector;
        }
        const float Scale = FMath::InvSqrt(SquareSum);
        return {X * Scale, Y * Scale, Z * Scale};
    }

    constexpr TVector<T> ComponentMin(const TVector<T>& Other) const
    {
        return {
            FMath::Min(X, Other.X),
            FMath::Min(Y, Other.Y),
            FMath::Min(Z, Other.Z)
        };
    }

    constexpr TVector<T> ComponentMax(const TVector<T>& Other) const
    {
        return {
            FMath::Max(X, Other.X),
            FMath::Max(Y, Other.Y),
            FMath::Max(Z, Other.Z)
        };
    }

    constexpr bool IsNearlyZero(T Tolerance = KINDA_SMALL_NUMBER) const
    {
        return FMath::Abs(X)<=Tolerance && FMath::Abs(Y)<=Tolerance && FMath::Abs(Z)<=Tolerance;
    }

    constexpr bool IsZero() const { return X==0 && Y==0 && Z==0; }

    // 런타임 함수(문자열 변환 등)는 constexpr 불가
    FString ToString() const
    {
        // FString::Printf를 사용하여 포맷팅된 문자열 생성
        // TEXT() 매크로는 리터럴 문자열을 TCHAR 타입으로 만들어줍니다.
        return FString::Printf(TEXT("X=%3.3f Y=%3.3f Z=%3.3f"), X, Y, Z);

        // 필요에 따라 소수점 정밀도 지정 가능: 예) "X=%.2f Y=%.2f Z=%.2f"
        // return FString::Printf(TEXT("X=%.2f Y=%.2f Z=%.2f"), x, y, z);
    }

    bool InitFromString(const FString& InSourceString)
    {
        // The initialization is only successful if the X, Y, and Z values can all be parsed from the string
        const bool bSuccessful = FParse::Value(*InSourceString, TEXT("X=") , X) && FParse::Value(*InSourceString, TEXT("Y="), Y) && FParse::Value(*InSourceString, TEXT("Z="), Z);

        return bSuccessful;
    }
};

// Static member definitions
template <typename T>
const TVector<T> TVector<T>::ZeroVector = TVector<T>(0, 0, 0);

template <typename T>
const TVector<T> TVector<T>::OneVector = TVector<T>(1, 1, 1);

template <typename T>
const TVector<T> TVector<T>::UpVector = TVector<T>(0, 0, 1);

template <typename T>
const TVector<T> TVector<T>::DownVector = TVector<T>(0, 0, -1);

template <typename T>
const TVector<T> TVector<T>::ForwardVector = TVector<T>(1, 0, 0);

template <typename T>
const TVector<T> TVector<T>::BackwardVector = TVector<T>(-1, 0, 0);

template <typename T>
const TVector<T> TVector<T>::RightVector = TVector<T>(0, 1, 0);

template <typename T>
const TVector<T> TVector<T>::LeftVector = TVector<T>(0, -1, 0);

template <typename T>
const TVector<T> TVector<T>::XAxisVector = TVector<T>(1, 0, 0);

template <typename T>
const TVector<T> TVector<T>::YAxisVector = TVector<T>(0, 1, 0);

template <typename T>
const TVector<T> TVector<T>::ZAxisVector = TVector<T>(0, 0, 1);

template <typename T>
FArchive& operator<<(FArchive& Ar, TVector<T>& V)
{
    return Ar << V.X << V.Y << V.Z;
}

// inline FVector::FVector(const FRotator& InRotator)
//     : X(FMath::DegreesToRadians(InRotator.Roll)), Y(FMath::DegreesToRadians(InRotator.Pitch)), Z(FMath::DegreesToRadians(InRotator.Yaw))
// {
// }

// inline float FVector::DistSquared(const FVector& V1, const FVector& V2)
// {
//     return FMath::Square(V2.X-V1.X) + FMath::Square(V2.Y-V1.Y) + FMath::Square(V2.Z-V1.Z);
// }

// inline float FVector::Distance(const FVector& V1, const FVector& V2)
// {
//     return FMath::Sqrt(
//         FMath::Square(V2.X - V1.X)
//         + FMath::Square(V2.Y - V1.Y)
//         + FMath::Square(V2.Z - V1.Z)
//     );
// }

// float FVector::DistSquaredXY(const FVector& V1, const FVector& V2)
// {
//     return FMath::Square(V2.X-V1.X) + FMath::Square(V2.Y-V1.Y);
// }

// float FVector::DistXY(const FVector& V1, const FVector& V2)
// {
//     return FMath::Sqrt(DistSquaredXY(V1, V2));
// }

// inline bool FVector::IsNormalized() const
// {
//     constexpr float ThreshVectorNormalized = 0.01f;
//     return (FMath::Abs(1.f - SizeSquared()) < ThreshVectorNormalized);
// }

// inline float FVector::operator|(const FVector& Other) const
// {
//     return X * Other.X + Y * Other.Y + Z * Other.Z;
// }

// inline float FVector::Dot(const FVector& Other) const
// {
//     return *this | Other;
// }

// inline float FVector::DotProduct(const FVector& A, const FVector& B)
// {
//     return A | B;
// }

// inline FVector FVector::operator^(const FVector& Other) const
// {
//     return {
//         Y * Other.Z - Z * Other.Y,
//         Z * Other.X - X * Other.Z,
//         X * Other.Y - Y * Other.X
//     };
// }

// inline FVector FVector::Cross(const FVector& Other) const
// {
//     return *this ^ Other;
// }

// inline FVector FVector::CrossProduct(const FVector& A, const FVector& B)
// {
//     return A ^ B;
// }

// inline FVector FVector::operator+(const FVector& Other) const
// {
//     return {X + Other.X, Y + Other.Y, Z + Other.Z};
// }

// inline FVector FVector::operator+(float Scalar) const
// {
//     return FVector{
//         X + Scalar,
//         Y + Scalar,
//         Z + Scalar
//     };
// }

// inline FVector& FVector::operator+=(const FVector& Other)
// {
//     X += Other.X; Y += Other.Y; Z += Other.Z;
//     return *this;
// }

// inline FVector FVector::operator-(const FVector& Other) const
// {
//     return {X - Other.X, Y - Other.Y, Z - Other.Z};
// }

// inline FVector FVector::operator-(float Scalar) const
// {
//     return FVector{
//         X - Scalar,
//         Y - Scalar,
//         Z - Scalar
//     };
// }

// inline FVector& FVector::operator-=(const FVector& Other)
// {
//     X -= Other.X; Y -= Other.Y; Z -= Other.Z;
//     return *this;
// }

// inline FVector FVector::operator*(const FVector& Other) const
// {
//     return {X * Other.X, Y * Other.Y, Z * Other.Z};
// }

// inline FVector FVector::operator*(float Scalar) const
// {
//     return {X * Scalar, Y * Scalar, Z * Scalar};
// }

// inline FVector& FVector::operator*=(const FVector& Other)
// {
//     X *= Other.X;
//     Y *= Other.Y;
//     Z *= Other.Z;
//     return *this;
// }

// inline FVector& FVector::operator*=(float Scalar)
// {
//     X *= Scalar; Y *= Scalar; Z *= Scalar;
//     return *this;
// }

// inline FVector FVector::operator/(const FVector& Other) const
// {
//     return {X / Other.X, Y / Other.Y, Z / Other.Z};
// }

// inline FVector FVector::operator/(float Scalar) const
// {
//     return {X / Scalar, Y / Scalar, Z / Scalar};
// }

// inline FVector& FVector::operator/=(float Scalar)
// {
//     X /= Scalar; Y /= Scalar; Z /= Scalar;
//     return *this;
// }

// inline FVector FVector::operator-() const
// {
//     return {-X, -Y, -Z};
// }

// inline bool FVector::operator==(const FVector& Other) const
// {
//     return X == Other.X && Y == Other.Y && Z == Other.Z;  // NOLINT(clang-diagnostic-float-equal)
// }

// inline bool FVector::operator!=(const FVector& Other) const
// {
//     return X != Other.X || Y != Other.Y || Z != Other.Z;  // NOLINT(clang-diagnostic-float-equal)
// }

// inline std::partial_ordering FVector::operator<=>(const FVector& Other) const
// {
//     return SizeSquared() <=> Other.SizeSquared();
// }

// inline std::partial_ordering FVector::operator<=>(float Scalar) const
// {
//     return SizeSquared() <=> Scalar * Scalar;
// }

// inline std::partial_ordering operator<=>(float Scalar, const FVector& Vector)
// {
//     return Scalar * Scalar <=> Vector.SizeSquared();
// }

// inline float& FVector::operator[](int Index)
// {
//     assert(0 <= Index && Index <= 2);
//     return reinterpret_cast<float*>(this)[Index];
// }

// inline const float& FVector::operator[](int Index) const
// {
//     assert(0 <= Index && Index <= 2);
//     return reinterpret_cast<const float*>(this)[Index];
// }

// inline bool FVector::ContainsNaN() const
// {
//     return (!FMath::IsFinite(X) || 
//             !FMath::IsFinite(Y) ||
//             !FMath::IsFinite(Z));
// }

// inline bool FVector::Equals(const FVector& V, float Tolerance) const
// {
//     return FMath::Abs(X-V.X) <= Tolerance && FMath::Abs(Y-V.Y) <= Tolerance && FMath::Abs(Z-V.Z) <= Tolerance;
// }

// inline bool FVector::AllComponentsEqual(float Tolerance) const
// {
//     return FMath::Abs(X - Y) <= Tolerance && FMath::Abs(X - Z) <= Tolerance && FMath::Abs(Y - Z) <= Tolerance;
// }

// inline float FVector::Length() const
// {
//     return FMath::Sqrt(X * X + Y * Y + Z * Z);
// }

// inline float FVector::SquaredLength() const
// {
//     return X * X + Y * Y + Z * Z;
// }

// inline bool FVector::Normalize(float Tolerance)
// {
//     const float SquareSum = X * X + Y * Y + Z * Z;
//     if (SquareSum > Tolerance)
//     {
//         const float Scale = FMath::InvSqrt(SquareSum);
//         X *= Scale; Y *= Scale; Z *= Scale;
//         return true;
//     }
//     return false;
// }

// inline float FVector::GetMin() const
// {
//     return FMath::Min(FMath::Min(X, Y), Z);
// }

// inline float FVector::GetMax() const
// {
//     return FMath::Max(FMath::Max(X, Y), Z);
// }

// inline FVector FVector::GetUnsafeNormal() const
// {
//     const float Scale = FMath::InvSqrt(X*X + Y*Y + Z*Z);
//     return {X * Scale, Y * Scale, Z * Scale};
// }

// inline FVector FVector::GetSafeNormal(float Tolerance) const
// {
//     const float SquareSum = X*X + Y*Y + Z*Z;

//     // Not sure if it's safe to add tolerance in there. Might introduce too many errors
//     if (SquareSum == 1.f)
//     {
//         return *this;
//     }
//     else if (SquareSum < Tolerance)
//     {
//         return ZeroVector;
//     }
//     const float Scale = FMath::InvSqrt(SquareSum);
//     return {X * Scale, Y * Scale, Z * Scale};
// }

// inline FVector FVector::ComponentMin(const FVector& Other) const
// {
//     return {
//         FMath::Min(X, Other.X),
//         FMath::Min(Y, Other.Y),
//         FMath::Min(Z, Other.Z)
//     };
// }

// inline FVector FVector::ComponentMax(const FVector& Other) const
// {
//     return {
//         FMath::Max(X, Other.X),
//         FMath::Max(Y, Other.Y),
//         FMath::Max(Z, Other.Z)
//     };
// }

// inline bool FVector::IsNearlyZero(float Tolerance) const
// {
//     return
//         FMath::Abs(X)<=Tolerance
//         && FMath::Abs(Y)<=Tolerance
//         && FMath::Abs(Z)<=Tolerance;
// }

// inline bool FVector::IsZero() const
// {
//     return X==0.f && Y==0.f && Z==0.f;
// }
