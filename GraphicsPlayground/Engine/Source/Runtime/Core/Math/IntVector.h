#pragma once

#include "MathUtility.h"
#include "MathFwd.h"
#include "Serialization/Archive.h"
#include "Misc/Parse.h"

template <typename T>
struct TIntVector3
{
    static_assert(std::is_integral_v<T>, "T must be integral");

    union
    {
        struct 
        {
            T X;
            T Y;
            T Z;
        };
        
        T XYZ[3];
    };

    constexpr TIntVector3() : X(0), Y(0), Z(0) {}
    constexpr TIntVector3(T X, T Y, T Z) : X(X), Y(Y), Z(Z) {}
    constexpr explicit TIntVector3(T Scalar) : X(Scalar), Y(Scalar), Z(Scalar) {}
    constexpr TIntVector3(const T&) = default;
    constexpr TIntVector3& operator=(const T&) = default;

    static const TIntVector3<T> ZeroVector;
    static const TIntVector3<T> OneVector;

public:
    constexpr const IntType& operator()(int32 ComponentIndex) const
    {
        return XYZ[ComponentIndex];
    }

    constexpr IntType& operator[](int32 ComponentIndex) const
    {
        return XYZ[ComponentIndex];
    }
    
    constexpr T SizeSquared() const
    {
        // Using int64 to prevent potential overflow for large integer values
        int64 LocalX64 = (int64)X;
        int64 LocalY64 = (int64)Y;
        int64 LocalZ64 = (int64)Z;

        return (T)(LocalX64 * LocalX64 + LocalY64 * LocalY64 + LocalZ64 * LocalZ64);
    }

    constexpr T Size() const
    {
        // Using int64 to prevent potential overflow for large integer values
        int64 LocalX64 = (int64)X;
        int64 LocalY64 = (int64)Y;
        int64 LocalZ64 = (int64)Z;

        return T(FMath::Sqrt((double)(LocalX64 * LocalX64 + LocalY64 * LocalY64 + LocalZ64 * LocalZ64)));
    }

    constexpr TIntVector3<T> operator+(const TIntVector3<T>& Other) const { return {X + Other.X, Y + Other.Y, Z + Other.Z}; }
    constexpr TIntVector3<T> operator+(T Scalar) const { return {X + Scalar, Y + Scalar, Z + Scalar}; }
    constexpr TIntVector3<T>& operator+=(const TIntVector3<T>& Other) { X += Other.X; Y += Other.Y; Z += Other.Z; return *this; }

    constexpr TIntVector3<T> operator-(const TIntVector3<T>& Other) const { return {X - Other.X, Y - Other.Y, Z - Other.Z}; }
    constexpr TIntVector3<T> operator-(T Scalar) const { return {X - Scalar, Y - Scalar, Z - Scalar}; }
    constexpr TIntVector3<T>& operator-=(const TIntVector3<T>& Other) { X -= Other.X; Y -= Other.Y; Z -= Other.Z; return *this; }

    constexpr TIntVector3<T> operator*(const TIntVector3<T>& Other) const { return {X * Other.X, Y * Other.Y, Z * Other.Z}; }
    constexpr TIntVector3<T> operator*(T Scalar) const { return {X * Scalar, Y * Scalar, Z * Scalar}; }
    constexpr TIntVector3<T>& operator*=(const TIntVector3<T>& Other) { X *= Other.X; Y *= Other.Y; Z *= Other.Z; return *this; }
    constexpr TIntVector3<T>& operator*=(T Scalar) { X *= Scalar; Y *= Scalar; Z *= Scalar; return *this; }

    constexpr TIntVector3<T> operator/(const TIntVector3<T>& Other) const { return {X / Other.X, Y / Other.Y, Z / Other.Z}; }
    constexpr TIntVector3<T> operator/(T Scalar) const { return {X / Scalar, Y / Scalar, Z / Scalar}; }
    constexpr TIntVector3<T>& operator/=(T Scalar) { X /= Scalar; Y /= Scalar; Z /= Scalar; return *this; }

    constexpr TIntVector3<T> operator-() const { return {-X, -Y, -Z}; }

    constexpr bool operator==(const TVector<T>& Other) const { return X == Other.X && Y == Other.Y && Z == Other.Z; }
    constexpr bool operator!=(const TVector<T>& Other) const { return X != Other.X || Y != Other.Y || Z != Other.Z; }

    constexpr std::partial_ordering operator<=>(const TVector<T>& Other) const { return SizeSquared() <=> Other.SizeSquared(); }
    constexpr std::partial_ordering operator<=>(T Scalar) const { return SizeSquared() <=> Scalar * Scalar; }

    constexpr T GetMin() const { return FMath::Min(FMath::Min(X, Y), Z); }
    constexpr T GetMax() const { return FMath::Max(FMath::Max(X, Y), Z); }

    constexpr TIntVector3<T> ComponentMin(const TIntVector3<T>& Other) const
    {
        return {
            FMath::Min(X, Other.X),
            FMath::Min(Y, Other.Y),
            FMath::Min(Z, Other.Z)
        };
    }

    constexpr TIntVector3<T> ComponentMax(const TIntVector3<T>& Other) const
    {
        return {
            FMath::Max(X, Other.X),
            FMath::Max(Y, Other.Y),
            FMath::Max(Z, Other.Z)
        };
    }
};

template <typename T>
struct TIntVector2
{
    static_assert(std::is_integral_v<T>, "T must be integral");

    union
    {
        struct
        {
            T X;
            T Y;
        };

        T XY[2];
    };

    constexpr TIntVector2() : X(0), Y(0) {}
    constexpr TIntVector2(T X, T Y) : X(X), Y(Y) {}
    constexpr explicit TIntVector2(T Scalar) : X(Scalar), Y(Scalar) {}
    constexpr TIntVector2(const T&) = default;
    constexpr TIntVector2& operator=(const T&) = default;

    static const TIntVector2<T> ZeroVector;
    static const TIntVector2<T> OneVector;

public:
    constexpr const IntType& operator()(int32 ComponentIndex) const
    {
        return XY[ComponentIndex];
    }

    constexpr IntType& operator[](int32 ComponentIndex) const
    {
        return XY[ComponentIndex];
    }

    constexpr T SizeSquared() const
    {
        int64 LocalX64 = (int64)X;
        int64 LocalY64 = (int64)Y;
        return (T)(LocalX64 * LocalX64 + LocalY64 * LocalY64);
    }

    constexpr T Size() const
    {
        int64 LocalX64 = (int64)X;
        int64 LocalY64 = (int64)Y;
        return T(FMath::Sqrt((double)(LocalX64 * LocalX64 + LocalY64 * LocalY64)));
    }

    constexpr TIntVector2<T> operator+(const TIntVector2<T>& Other) const { return {X + Other.X, Y + Other.Y}; }
    constexpr TIntVector2<T> operator+(T Scalar) const { return {X + Scalar, Y + Scalar}; }
    constexpr TIntVector2<T>& operator+=(const TIntVector2<T>& Other) { X += Other.X; Y += Other.Y; return *this; }

    constexpr TIntVector2<T> operator-(const TIntVector2<T>& Other) const { return {X - Other.X, Y - Other.Y}; }
    constexpr TIntVector2<T> operator-(T Scalar) const { return {X - Scalar, Y - Scalar}; }
    constexpr TIntVector2<T>& operator-=(const TIntVector2<T>& Other) { X -= Other.X; Y -= Other.Y; return *this; }

    constexpr TIntVector2<T> operator*(const TIntVector2<T>& Other) const { return {X * Other.X, Y * Other.Y}; }
    constexpr TIntVector2<T> operator*(T Scalar) const { return {X * Scalar, Y * Scalar}; }
    constexpr TIntVector2<T>& operator*=(const TIntVector2<T>& Other) { X *= Other.X; Y *= Other.Y; return *this; }
    constexpr TIntVector2<T>& operator*=(T Scalar) { X *= Scalar; Y *= Scalar; return *this; }

    constexpr TIntVector2<T> operator/(const TIntVector2<T>& Other) const { return {X / Other.X, Y / Other.Y}; }
    constexpr TIntVector2<T> operator/(T Scalar) const { return {X / Scalar, Y / Scalar}; }
    constexpr TIntVector2<T>& operator/=(T Scalar) { X /= Scalar; Y /= Scalar; return *this; }

    constexpr TIntVector2<T> operator-() const { return {-X, -Y}; }

    constexpr bool operator==(const TVector<T>& Other) const { return X == Other.X && Y == Other.Y; }
    constexpr bool operator!=(const TVector<T>& Other) const { return X != Other.X || Y != Other.Y; }

    constexpr std::partial_ordering operator<=>(const TVector<T>& Other) const { return SizeSquared() <=> Other.SizeSquared(); }
    constexpr std::partial_ordering operator<=>(T Scalar) const { return SizeSquared() <=> Scalar * Scalar; }

    constexpr T GetMin() const { return FMath::Min(X, Y); }
    constexpr T GetMax() const { return FMath::Max(X, Y); }

    constexpr TIntVector2<T> ComponentMin(const TIntVector2<T>& Other) const
    {
        return { FMath::Min(X, Other.X), FMath::Min(Y, Other.Y) };
    }

    constexpr TIntVector2<T> ComponentMax(const TIntVector2<T>& Other) const
    {
        return { FMath::Max(X, Other.X), FMath::Max(Y, Other.Y) };
    }
};

template <typename T>
struct TIntVector4
{
    static_assert(std::is_integral_v<T>, "T must be integral");

    union
    {
        struct
        {
            T X;
            T Y;
            T Z;
            T W;
        };

        T XYZW[4];
    };

    constexpr TIntVector4() : X(0), Y(0), Z(0), W(0) {}
    constexpr TIntVector4(T X, T Y, T Z, T W) : X(X), Y(Y), Z(Z), W(W) {}
    constexpr explicit TIntVector4(T Scalar) : X(Scalar), Y(Scalar), Z(Scalar), W(Scalar) {}
    constexpr TIntVector4(const T&) = default;
    constexpr TIntVector4& operator=(const T&) = default;

    static const TIntVector4<T> ZeroVector;
    static const TIntVector4<T> OneVector;

public:
    constexpr const IntType& operator()(int32 ComponentIndex) const
    {
        return XYZW[ComponentIndex];
    }

    constexpr IntType& operator[](int32 ComponentIndex) const
    {
        return XYZW[ComponentIndex];
    }

    constexpr T SizeSquared() const
    {
        int64 LocalX64 = (int64)X;
        int64 LocalY64 = (int64)Y;
        int64 LocalZ64 = (int64)Z;
        int64 LocalW64 = (int64)W;
        return (T)(LocalX64 * LocalX64 + LocalY64 * LocalY64 + LocalZ64 * LocalZ64 + LocalW64 * LocalW64);
    }

    constexpr T Size() const
    {
        int64 LocalX64 = (int64)X;
        int64 LocalY64 = (int64)Y;
        int64 LocalZ64 = (int64)Z;
        int64 LocalW64 = (int64)W;
        return T(FMath::Sqrt((double)(LocalX64 * LocalX64 + LocalY64 * LocalY64 + LocalZ64 * LocalZ64 + LocalW64 * LocalW64)));
    }

    constexpr TIntVector4<T> operator+(const TIntVector4<T>& Other) const { return {X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W}; }
    constexpr TIntVector4<T> operator+(T Scalar) const { return {X + Scalar, Y + Scalar, Z + Scalar, W + Scalar}; }
    constexpr TIntVector4<T>& operator+=(const TIntVector4<T>& Other) { X += Other.X; Y += Other.Y; Z += Other.Z; W += Other.W; return *this; }

    constexpr TIntVector4<T> operator-(const TIntVector4<T>& Other) const { return {X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W}; }
    constexpr TIntVector4<T> operator-(T Scalar) const { return {X - Scalar, Y - Scalar, Z - Scalar, W - Scalar}; }
    constexpr TIntVector4<T>& operator-=(const TIntVector4<T>& Other) { X -= Other.X; Y -= Other.Y; Z -= Other.Z; W -= Other.W; return *this; }

    constexpr TIntVector4<T> operator*(const TIntVector4<T>& Other) const { return {X * Other.X, Y * Other.Y, Z * Other.Z, W * Other.W}; }
    constexpr TIntVector4<T> operator*(T Scalar) const { return {X * Scalar, Y * Scalar, Z * Scalar, W * Scalar}; }
    constexpr TIntVector4<T>& operator*=(const TIntVector4<T>& Other) { X *= Other.X; Y *= Other.Y; Z *= Other.Z; W *= Other.W; return *this; }
    constexpr TIntVector4<T>& operator*=(T Scalar) { X *= Scalar; Y *= Scalar; Z *= Scalar; W *= Scalar; return *this; }

    constexpr TIntVector4<T> operator/(const TIntVector4<T>& Other) const { return {X / Other.X, Y / Other.Y, Z / Other.Z, W / Other.W}; }
    constexpr TIntVector4<T> operator/(T Scalar) const { return {X / Scalar, Y / Scalar, Z / Scalar, W / Scalar}; }
    constexpr TIntVector4<T>& operator/=(T Scalar) { X /= Scalar; Y /= Scalar; Z /= Scalar; W /= Scalar; return *this; }

    constexpr TIntVector4<T> operator-() const { return {-X, -Y, -Z, -W}; }

    constexpr bool operator==(const TVector<T>& Other) const { return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W; }
    constexpr bool operator!=(const TVector<T>& Other) const { return X != Other.X || Y != Other.Y || Z != Other.Z || W != Other.W; }

    constexpr std::partial_ordering operator<=>(const TVector<T>& Other) const { return SizeSquared() <=> Other.SizeSquared(); }
    constexpr std::partial_ordering operator<=>(T Scalar) const { return SizeSquared() <=> Scalar * Scalar; }

    constexpr T GetMin() const { return FMath::Min(FMath::Min(X, Y), FMath::Min(Z, W)); }
    constexpr T GetMax() const { return FMath::Max(FMath::Max(X, Y), FMath::Max(Z, W)); }

    constexpr TIntVector4<T> ComponentMin(const TIntVector4<T>& Other) const
    {
        return {
            FMath::Min(X, Other.X),
            FMath::Min(Y, Other.Y),
            FMath::Min(Z, Other.Z),
            FMath::Min(W, Other.W)
        };
    }

    constexpr TIntVector4<T> ComponentMax(const TIntVector4<T>& Other) const
    {
        return {
            FMath::Max(X, Other.X),
            FMath::Max(Y, Other.Y),
            FMath::Max(Z, Other.Z),
            FMath::Max(W, Other.W)
        };
    }
};

template<typename T>
const TIntVector3<T> TIntVector3<T>::ZeroVector = TIntVector3<T>(T(0), T(0), T(0));

template<typename T>
const TIntVector3<T> TIntVector3<T>::OneVector = TIntVector3<T>(T(1), T(1), T(1));

template<typename T>
const TIntVector2<T> TIntVector2<T>::ZeroVector = TIntVector2<T>(T(0), T(0));

template<typename T>
const TIntVector2<T> TIntVector2<T>::OneVector = TIntVector2<T>(T(1), T(1));

template<typename T>
const TIntVector4<T> TIntVector4<T>::ZeroVector = TIntVector4<T>(T(0), T(0), T(0), T(0));

template<typename T>
const TIntVector4<T> TIntVector4<T>::OneVector = TIntVector4<T>(T(1), T(1), T(1), T(1));