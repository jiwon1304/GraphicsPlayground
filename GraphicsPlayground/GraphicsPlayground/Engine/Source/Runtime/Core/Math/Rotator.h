#pragma once
#include "Container/String.h"
#include "Serialization/Archive.h"
#include "MathUtility.h"
#include "Vector.h"
#include "Matrix.h"
#include "Misc/Parse.h"

struct FVector;
struct FQuat;
struct FMatrix;

// 회전 정보를 Degree 단위로 저장하는 템플릿 구조체
template <typename T>
struct TRotator
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");

    T Pitch;
    T Yaw;
    T Roll;

    explicit TRotator()
        : Pitch(0), Yaw(0), Roll(0)
    {}

    explicit TRotator(T InPitch, T InYaw, T InRoll)
        : Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
    {}

    TRotator(const TRotator& Other) = default;

    explicit TRotator(const TVector<T>& InVector);
    explicit TRotator(const TQuat<T>& InQuat);

    // Zero rotator (C++17 inline static)
    inline static const TRotator<T> ZeroRotator{};

    TRotator operator+(const TRotator& Other) const;
    TRotator& operator+=(const TRotator& Other);

    TRotator operator-(const TRotator& Other) const;
    TRotator& operator-=(const TRotator& Other);

    TRotator operator*(T Scalar) const;
    TRotator& operator*=(T Scalar);

    friend TRotator operator*(T Scalar, const TRotator& Rotator)
    {
        return Rotator * Scalar; // 기존 멤버 함수 재활용
    }

    TRotator operator/(const TRotator& Other) const;
    TRotator operator/(T Scalar) const;
    TRotator& operator/=(T Scalar);

    TRotator operator-() const;

    bool operator==(const TRotator& Other) const;
    bool operator!=(const TRotator& Other) const;

    bool IsNearlyZero(T Tolerance = KINDA_SMALL_NUMBER) const;
    bool IsZero() const;

    bool Equals(const TRotator& Other, T Tolerance = KINDA_SMALL_NUMBER) const;

    TRotator Add(T DeltaPitch, T DeltaYaw, T DeltaRoll) const;

    TRotator FromQuaternion(const TQuat<T>& InQuat) const;
    TQuat<T> Quaternion() const;
    TVector<T> ToVector() const;
    TVector<T> RotateVector(const TVector<T>& Vec) const;
    FMatrix ToMatrix() const;

    static T ClampAxis(T Angle);
    TRotator GetNormalized() const;
    void Normalize();

    FString ToString() const;
    bool InitFromString(const FString& InSourceString);

    static T NormalizeAxis(T Angle);
    static TRotator MakeLookAtRotation(const TVector<T>& From, const TVector<T>& To);
};

template <typename T>
inline FArchive& operator<<(FArchive& Ar, TRotator<T>& R)
{
    Ar << R.Pitch << R.Yaw << R.Roll;
    return Ar;
}

// ===== Template definitions =====
template <typename T>
TRotator<T>::TRotator(const TVector<T>& InVector)
    : Pitch(FMath::RadiansToDegrees(InVector.Y)), Yaw(FMath::RadiansToDegrees(InVector.Z)), Roll(FMath::RadiansToDegrees(InVector.X))
{
}

// template <typename T>
// TRotator<T>::TRotator(const TQuat<T>& InQuat)
// {
//     const FRotator R = InQuat.Rotator();
//     Pitch = static_cast<T>(R.Pitch);
//     Yaw   = static_cast<T>(R.Yaw);
//     Roll  = static_cast<T>(R.Roll);
// }

template <typename T>
TRotator<T> TRotator<T>::operator+(const TRotator& Other) const
{
    return TRotator(Pitch + Other.Pitch, Yaw + Other.Yaw, Roll + Other.Roll);
}

template <typename T>
TRotator<T>& TRotator<T>::operator+=(const TRotator& Other)
{
    Pitch += Other.Pitch; Yaw += Other.Yaw; Roll += Other.Roll;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator-(const TRotator& Other) const
{
    return TRotator(Pitch - Other.Pitch, Yaw - Other.Yaw, Roll - Other.Roll);
}

template <typename T>
TRotator<T>& TRotator<T>::operator-=(const TRotator& Other)
{
    Pitch -= Other.Pitch; Yaw -= Other.Yaw; Roll -= Other.Roll;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator*(T Scalar) const
{
    return TRotator(Pitch * Scalar, Yaw * Scalar, Roll * Scalar);
}

template <typename T>
TRotator<T>& TRotator<T>::operator*=(T Scalar)
{
    Pitch *= Scalar; Yaw *= Scalar; Roll *= Scalar;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator/(const TRotator& Other) const
{
    return TRotator(Pitch / Other.Pitch, Yaw / Other.Yaw, Roll / Other.Roll);
}

template <typename T>
TRotator<T> TRotator<T>::operator/(T Scalar) const
{
    return TRotator(Pitch / Scalar, Yaw / Scalar, Roll / Scalar);
}

template <typename T>
TRotator<T>& TRotator<T>::operator/=(T Scalar)
{
    Pitch /= Scalar; Yaw /= Scalar; Roll /= Scalar;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator-() const
{
    return TRotator(-Pitch, -Yaw, -Roll);
}

template <typename T>
bool TRotator<T>::operator==(const TRotator& Other) const
{
    return Pitch == Other.Pitch && Yaw == Other.Yaw && Roll == Other.Roll;
}

template <typename T>
bool TRotator<T>::operator!=(const TRotator& Other) const
{
    return Pitch != Other.Pitch || Yaw != Other.Yaw || Roll != Other.Roll;
}

template <typename T>
bool TRotator<T>::IsNearlyZero(T Tolerance) const
{
    return FMath::Abs(Pitch) <= Tolerance && FMath::Abs(Yaw) <= Tolerance && FMath::Abs(Roll) <= Tolerance;
}

template <typename T>
bool TRotator<T>::IsZero() const
{
    return Pitch == T(0) && Yaw == T(0) && Roll == T(0);
}

template <typename T>
bool TRotator<T>::Equals(const TRotator& Other, T Tolerance) const
{
    return FMath::Abs(Pitch - Other.Pitch) <= Tolerance && FMath::Abs(Yaw - Other.Yaw) <= Tolerance && FMath::Abs(Roll - Other.Roll) <= Tolerance;
}

template <typename T>
TRotator<T> TRotator<T>::Add(T DeltaPitch, T DeltaYaw, T DeltaRoll) const
{
    return TRotator(Pitch + DeltaPitch, Yaw + DeltaYaw, Roll + DeltaRoll);
}

// template <typename T>
// TRotator<T> TRotator<T>::FromQuaternion(const TQuat<T>& InQuat) const
// {
//     return TRotator(InQuat);
// }

// template <typename T>
// TQuat<T> TRotator<T>::Quaternion() const
// {
//     const T DegToRad = static_cast<T>(PI) / T(180);
//     const T Div = DegToRad / T(2);
//     T SP, SY, SR;
//     T CP, CY, CR;

//     const T PitchNoWinding = FMath::Fmod(Pitch, T(360));
//     const T YawNoWinding = FMath::Fmod(Yaw, T(360));
//     const T RollNoWinding = FMath::Fmod(Roll, T(360));

//     FMath::SinCos(&SP, &CP, PitchNoWinding * Div);
//     FMath::SinCos(&SY, &CY, YawNoWinding * Div);
//     FMath::SinCos(&SR, &CR, RollNoWinding * Div);
    
//     TQuat<T> RotationQuat;
//     RotationQuat.X = CR * SP * SY - SR * CP * CY;
//     RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
//     RotationQuat.Z = CR * CP * SY - SR * SP * CY;
//     RotationQuat.W = CR * CP * CY + SR * SP * SY;

//     return RotationQuat;
// }

template <typename T>
TVector<T> TRotator<T>::ToVector() const
{
    const T PitchNoWinding = FMath::Fmod(Pitch, T(360));
    const T YawNoWinding = FMath::Fmod(Yaw, T(360));

    T CP, SP, CY, SY;
    FMath::SinCos(&SP, &CP, FMath::DegreesToRadians(PitchNoWinding));
    FMath::SinCos(&SY, &CY, FMath::DegreesToRadians(YawNoWinding));
    TVector<T> V(CP*CY, CP*SY, SP);

    if (!FMath::IsFinite(static_cast<float>(V.X)) || !FMath::IsFinite(static_cast<float>(V.Y)) || !FMath::IsFinite(static_cast<float>(V.Z)))
    {
        V = TVector<T>::ForwardVector;
    }
    
    return V;
}

template <typename T>
TVector<T> TRotator<T>::RotateVector(const TVector<T>& Vec) const
{
    return Quaternion().RotateVector(Vec);
}

template <typename T>
FMatrix TRotator<T>::ToMatrix() const
{
    // FMatrix는 FRotator에 대한 오버로드가 있으므로 필요 시 변환
    return FMatrix::CreateRotationMatrix(FRotator(static_cast<float>(Pitch), static_cast<float>(Yaw), static_cast<float>(Roll)));
}

template <typename T>
TRotator<T> TRotator<T>::MakeLookAtRotation(const TVector<T>& From, const TVector<T>& To)
{
    TVector<T> Dir = To - From;
    T Yaw = FMath::Atan2(Dir.Y, Dir.X) * (T(180) / static_cast<T>(PI));
    T DistanceXY = FMath::Sqrt(Dir.X * Dir.X + Dir.Y * Dir.Y);
    T Pitch = FMath::Atan2(Dir.Z, DistanceXY) * (T(180) / static_cast<T>(PI));
    T Roll = T(0);
    return TRotator(Pitch, Yaw, Roll);
}

template <typename T>
T TRotator<T>::ClampAxis(T Angle)
{
    Angle = FMath::Fmod(Angle, T(360));
    if (Angle < T(0))
    {
        Angle += T(360);
    }
    return Angle;
}

template <typename T>
TRotator<T> TRotator<T>::GetNormalized() const
{
    return TRotator(FMath::UnwindDegrees(Pitch), FMath::UnwindDegrees(Yaw), FMath::UnwindDegrees(Roll));
}

template <typename T>
void TRotator<T>::Normalize()
{
    Pitch = FMath::UnwindDegrees(Pitch);
    Yaw = FMath::UnwindDegrees(Yaw);
    Roll = FMath::UnwindDegrees(Roll);
}

template <typename T>
FString TRotator<T>::ToString() const
{
    return FString::Printf(TEXT("Pitch=%3.3f Yaw=%3.3f Roll=%3.3f"), static_cast<float>(Pitch), static_cast<float>(Yaw), static_cast<float>(Roll));
}

template <typename T>
bool TRotator<T>::InitFromString(const FString& InSourceString)
{
    Pitch = T(0);
    Yaw = T(0);
    Roll = T(0);

    float FP=0, FY=0, FR=0;
    const bool bSuccess = FParse::Value(*InSourceString, TEXT("Pitch="), FP) &&
        FParse::Value(*InSourceString, TEXT("Yaw="), FY) &&
        FParse::Value(*InSourceString, TEXT("Roll="), FR);

    if (bSuccess)
    {
        Pitch = static_cast<T>(FP);
        Yaw   = static_cast<T>(FY);
        Roll  = static_cast<T>(FR);
    }
    return bSuccess;
}

template <typename T>
T TRotator<T>::NormalizeAxis(T Angle)
{
    Angle = ClampAxis(Angle);

    if (Angle > T(180))
    {
        // shift to (-180,180]
        Angle -= T(360);
    }

    return Angle;
}
