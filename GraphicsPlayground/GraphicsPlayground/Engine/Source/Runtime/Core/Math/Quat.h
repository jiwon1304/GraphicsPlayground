#pragma once
#include "MathUtility.h"
#include "MathFwd.h"
#include "Matrix.h"
#include "Serialization/Archive.h"

/**
 * 4차원 복소수(quaternion)를 표현하는 구조체입니다. 회전 변환에 주로 사용됩니다.
 */
template <typename T>
struct TQuat
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
    alignas(16) T X, Y, Z, W;

    using FReal = T;

    static const TQuat<T> Identity;

public:
    // 기본 생성자 (항등 쿼터니언: X=0, Y=0, Z=0, W=1)
    constexpr TQuat() : X(0), Y(0), Z(0), W(1) {}

    // X, Y, Z, W 값으로 초기화
    constexpr TQuat(T InX, T InY, T InZ, T InW) : X(InX), Y(InY), Z(InZ), W(InW) {}

    constexpr TQuat(const TQuat&) = default;
    constexpr TQuat& operator=(const TQuat&) = default;

    // 회전 축과 각도(라디안)를 받아서 TQuat 생성
    FORCEINLINE TQuat(const TVector<T>& Axis, T AngleRad)
    {
        *this = FromAxisAngle(Axis, AngleRad);
    }

    // 회전 행렬로부터 TQuat 생성
    TQuat(const FMatrix& M)
    {
        const T Trace = static_cast<T>(M.M[0][0] + M.M[1][1] + M.M[2][2]);

        if (Trace > T(0))
        {
            const T S = FMath::Sqrt(Trace + T(1)) * T(2);
            W = T(0.25) * S;
            X = static_cast<T>(M.M[1][2] - M.M[2][1]) / S;
            Y = static_cast<T>(M.M[2][0] - M.M[0][2]) / S;
            Z = static_cast<T>(M.M[0][1] - M.M[1][0]) / S;
        }
        else if ((M.M[0][0] > M.M[1][1]) && (M.M[0][0] > M.M[2][2]))
        {
            const T S = FMath::Sqrt(T(1) + static_cast<T>(M.M[0][0] - M.M[1][1] - M.M[2][2])) * T(2);
            W = static_cast<T>(M.M[1][2] - M.M[2][1]) / S;
            X = T(0.25) * S;
            Y = static_cast<T>(M.M[1][0] + M.M[0][1]) / S;
            Z = static_cast<T>(M.M[2][0] + M.M[0][2]) / S;
        }
        else if (M.M[1][1] > M.M[2][2])
        {
            const T S = FMath::Sqrt(T(1) + static_cast<T>(M.M[1][1] - M.M[0][0] - M.M[2][2])) * T(2);
            W = static_cast<T>(M.M[2][0] - M.M[0][2]) / S;
            X = static_cast<T>(M.M[1][0] + M.M[0][1]) / S;
            Y = T(0.25) * S;
            Z = static_cast<T>(M.M[2][1] + M.M[1][2]) / S;
        }
        else
        {
            const T S = FMath::Sqrt(T(1) + static_cast<T>(M.M[2][2] - M.M[0][0] - M.M[1][1])) * T(2);
            W = static_cast<T>(M.M[0][1] - M.M[1][0]) / S;
            X = static_cast<T>(M.M[2][0] + M.M[0][2]) / S;
            Y = static_cast<T>(M.M[2][1] + M.M[1][2]) / S;
            Z = T(0.25) * S;
        }
    }

    // 오일러 각(FRotator)으로부터 TQuat 생성 (Pitch, Yaw, Roll 순서로 도(degree) 단위 입력)
    explicit TQuat(const TRotator<T>& R);

public:
    /**
     * 벡터 A에서 B로의 최소 회전을 표현하는 쿼터니언을 계산합니다.
     *
     * @param A 시작 벡터
     * @param B 목표 벡터
     * @return 정규화된 단위 쿼터니언
     *
     * @warning 입력 벡터가 영벡터인 경우 정의되지 않은 동작
     * @note 평행/반대 방향 벡터 처리:
     *  - 평행 시: 임의의 수직축 기반 회전
     *  - 반대 시: X/Y/Z 축 중 최대 요소 기반 직교축 사용
     */
    static TQuat<T> FindBetween(const TVector<T>& A, const TVector<T>& B)
    {
        const T NormAB = FMath::Sqrt(A.SizeSquared() * B.SizeSquared());
        T W = NormAB + TVector<T>::DotProduct(A, B);
        TQuat<T> Result;

        if (W >= T(1e-6) * NormAB)
        {
            Result = TQuat<T>(
                A.Y * B.Z - A.Z * B.Y,
                A.Z * B.X - A.X * B.Z,
                A.X * B.Y - A.Y * B.X,
                W
            );
        }
        else
        {
            W = T(0);
            const T X = FMath::Abs(A.X);
            const T Y = FMath::Abs(A.Y);
            const T Z = FMath::Abs(A.Z);

            const TVector<T> Basis = (X > Y && X > Z) ? TVector<T>::YAxisVector : -TVector<T>::XAxisVector;

            Result = TQuat<T>(
                A.Y * Basis.Z - A.Z * Basis.Y,
                A.Z * Basis.X - A.X * Basis.Z,
                A.X * Basis.Y - A.Y * Basis.X,
                W
            );
        }

        Result.Normalize();
        return Result;
    }

    // 쿼터니언의 곱셈 연산 (회전 결합)
    TQuat<T> operator*(const TQuat<T>& Other) const
    {
        return TQuat<T>{
            W * Other.X + X * Other.W + Y * Other.Z - Z * Other.Y,  // New X
            W * Other.Y - X * Other.Z + Y * Other.W + Z * Other.X,  // New Y
            W * Other.Z + X * Other.Y - Y * Other.X + Z * Other.W,  // New Z
            W * Other.W - X * Other.X - Y * Other.Y - Z * Other.Z   // New W
        };
    }

    TQuat<T> operator*(T Scale) const
    {
        return TQuat<T>{Scale * X, Scale * Y, Scale * Z, Scale * W};
    }

    T operator|(const TQuat<T>& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z + W * Other.W;
    }

    bool operator==(const TQuat<T>& Q) const
    {
        return Equals(Q, SMALL_NUMBER);
    }

    // (쿼터니언) 벡터 회전
    TVector<T> RotateVector(const TVector<T>& V) const
    {
        const TQuat<T> Conjugate = TQuat<T>(-X, -Y, -Z, W);
        const TQuat<T> VQuat(V.X, V.Y, V.Z, T(0));
        const TQuat<T> Temp = *this * VQuat;
        const TQuat<T> Result = Temp * Conjugate;
        return TVector<T>{Result.X, Result.Y, Result.Z};
    }

    // 단위 쿼터니언 여부 확인
    bool IsNormalized() const
    {
        return FMath::Abs(X * X + Y * Y + Z * Z + W * W - T(1)) < KINDA_SMALL_NUMBER;
    }

    // 쿼터니언 정규화 (단위 쿼터니언으로 만듬)
    void Normalize(T Tolerance = SMALL_NUMBER)
    {
        const T SquareSum = X * X + Y * Y + Z * Z + W * W;
        if (SquareSum >= Tolerance)
        {
            const T Scale = FMath::InvSqrt(SquareSum);
            X *= Scale;
            Y *= Scale;
            Z *= Scale;
            W *= Scale;
        }
        else
        {
            *this = Identity;
        }
    }

    /** 정규화된 쿼터니언을 가져옵니다. */
    TQuat<T> GetNormalized(T Tolerance = SMALL_NUMBER) const
    {
        TQuat<T> Result(*this);
        Result.Normalize(Tolerance);
        return Result;
    }

    /**
     * 쿼터니언을 회전축과 각도(라디안)로 분해합니다.
     * 
     * @param Axis 회전축 단위벡터
     * @param Angle 라디안 단위 회전각 (0 <= Angle <= 2π)
     */
    void ToAxisAndAngle(TVector<T>& Axis, T& Angle) const
    {
        Angle = GetAngle();
        Axis = GetRotationAxis();
    }

    /**
    * 쿼터니언의 회전각을 라디안 단위로 반환합니다.
    * 
    * @return 0 ~ 2π 범위의 회전각
    * @note 내부 계산식: 2 * acos(W)
    */
    T GetAngle() const
    {
        return T(2) * FMath::Acos(FMath::Clamp(W, T(-1), T(1)));
    }

    /**
     * 쿼터니언의 회전축을 계산합니다.
     * 
     * @return 정규화된 회전축 단위벡터
     * @warning 영쿼터니언(W=±1) 입력 시 X축 반환
     */
    TVector<T> GetRotationAxis() const
    {
        const T SquareSum = X*X + Y*Y + Z*Z;
        if (SquareSum < SMALL_NUMBER)
        {
            return TVector<T>::XAxisVector;
        }
        const T Scale = FMath::InvSqrt(SquareSum);
        return TVector<T>{X * Scale, Y * Scale, Z * Scale};
    }

    /** TQuat가 오차값 이내로 같은지 확인합니다. */
    bool Equals(const TQuat<T>& Q, T Tolerance=KINDA_SMALL_NUMBER) const
    {
        return (FMath::Abs(X - Q.X) <= Tolerance && FMath::Abs(Y - Q.Y) <= Tolerance && FMath::Abs(Z - Q.Z) <= Tolerance && FMath::Abs(W - Q.W) <= Tolerance)
            || (FMath::Abs(X + Q.X) <= Tolerance && FMath::Abs(Y + Q.Y) <= Tolerance && FMath::Abs(Z + Q.Z) <= Tolerance && FMath::Abs(W + Q.W) <= Tolerance);
    }

    /**
     * 다른 쿼터니언과의 각도 거리를 계산합니다.
     *
     * @param Q 비교 대상 쿼터니언
     * @return 두 쿼터니언의 회전 각 차이(라디안 단위, 0 ~ π 범위)
     *
     * @note 내부 계산식: acos(2*(Q·this)^2 - 1)
     */
    T AngularDistance(const TQuat<T>& Q) const
    {
        const T InnerProd = X*Q.X + Y*Q.Y + Z*Q.Z + W*Q.W;
        return FMath::Acos(FMath::Clamp((T(2) * InnerProd * InnerProd) - T(1), T(-1), T(1)));
    }

    bool ContainsNaN() const
    {
        return FMath::IsNaN(X) || FMath::IsNaN(Y) || FMath::IsNaN(Z) || FMath::IsNaN(W);
    }

    static TQuat<T> Slerp_NotNormalized(const TQuat<T>& Quat1, const TQuat<T>& Quat2, T Slerp)
    {
        T RawCosom =
            Quat1.X * Quat2.X +
            Quat1.Y * Quat2.Y +
            Quat1.Z * Quat2.Z +
            Quat1.W * Quat2.W;

        const T Sign = FMath::FloatSelect(RawCosom, T(1), T(-1));
        RawCosom *= Sign;
            
        T Scale0 = T(1) - Slerp;
        T Scale1 = Slerp * Sign;
            
        if (RawCosom < T(0.9999))
        {
            const T Omega = FMath::Acos(RawCosom);
            const T InvSin = T(1) / FMath::Sin(Omega);
            Scale0 = FMath::Sin(Scale0 * Omega) * InvSin;
            Scale1 = FMath::Sin(Scale1 * Omega) * InvSin;
        }
            
        return TQuat<T>{
            Scale0 * Quat1.X + Scale1 * Quat2.X,
            Scale0 * Quat1.Y + Scale1 * Quat2.Y,
            Scale0 * Quat1.Z + Scale1 * Quat2.Z,
            Scale0 * Quat1.W + Scale1 * Quat2.W
        };
    }

    static FORCEINLINE TQuat<T> Slerp(const TQuat<T>& Quat1, const TQuat<T>& Quat2, T Slerp)
    {
        return Slerp_NotNormalized(Quat1, Quat2, Slerp).GetNormalized();
    }

    // 회전 각도와 축으로부터 쿼터니언 생성 (axis-angle 방식)
    static TQuat<T> FromAxisAngle(const TVector<T>& Axis, T AngleRad)
    {
        const T HalfAngle = AngleRad * T(0.5);
        const T SinHalfAngle = FMath::Sin(HalfAngle);
        const T CosHalfAngle = FMath::Cos(HalfAngle);

        return TQuat<T>{
            Axis.X * SinHalfAngle,
            Axis.Y * SinHalfAngle,
            Axis.Z * SinHalfAngle,
            CosHalfAngle
        };
    }

    // 오일러 각(Roll, Pitch, Yaw - 도 단위)으로부터 쿼터니언 생성. 회전 순서는 Z(Yaw) -> Y(Pitch) -> X(Roll).
    static TQuat<T> MakeFromEuler(const TVector<T>& EulerDegrees) // EulerDegrees.X = Roll, .Y = Pitch, .Z = Yaw
    {
        constexpr T DegreeToRadian = static_cast<T>(PI) / T(180);
        const T HalfRoll  = (EulerDegrees.X * DegreeToRadian) * T(0.5);
        const T HalfPitch = (EulerDegrees.Y * DegreeToRadian) * T(0.5);
        const T HalfYaw   = (EulerDegrees.Z * DegreeToRadian) * T(0.5);

        const T SR = FMath::Sin(HalfRoll);  const T CR = FMath::Cos(HalfRoll);
        const T SP = FMath::Sin(HalfPitch); const T CP = FMath::Cos(HalfPitch);
        const T SY = FMath::Sin(HalfYaw);   const T CY = FMath::Cos(HalfYaw);
        
        return TQuat<T>{
            CR*SP*CY + SR*CP*SY,
            CR*CP*SY - SR*SP*CY,
            SR*CP*CY - CR*SP*SY,
            CR*CP*CY + SR*SP*SY
        };
    }


    // 쿼터니언을 회전 행렬로 변환
    FMatrix ToMatrix() const
    {
        FMatrix R;

        const T X2 = X + X;    const T Y2 = Y + Y;    const T Z2 = Z + Z;
        const T XX = X * X2;   const T XY = X * Y2;   const T XZ = X * Z2;
        const T YY = Y * Y2;   const T YZ = Y * Z2;   const T ZZ = Z * Z2;
        const T WX = W * X2;   const T WY = W * Y2;   const T WZ = W * Z2;

        R.M[0][0] = static_cast<float>(T(1) - (YY + ZZ));    R.M[1][0] = static_cast<float>(XY - WZ);               R.M[2][0] = static_cast<float>(XZ + WY);             R.M[3][0] = 0.0f;
        R.M[0][1] = static_cast<float>(XY + WZ);             R.M[1][1] = static_cast<float>(T(1) - (XX + ZZ));      R.M[2][1] = static_cast<float>(YZ - WX);             R.M[3][1] = 0.0f;
        R.M[0][2] = static_cast<float>(XZ - WY);             R.M[1][2] = static_cast<float>(YZ + WX);               R.M[2][2] = static_cast<float>(T(1) - (XX + YY));    R.M[3][2] = 0.0f;
        
        R.M[0][3] = 0.0f;                R.M[1][3] = 0.0f;                  R.M[2][3] = 0.0f;                R.M[3][3] = 1.0f;

        return R;
    }


    // 쿼터니언을 FRotator (오일러 각, 도 단위)로 변환
    TRotator<T> Rotator() const;

    TQuat<T> Inverse() const
    {
        return TQuat<T>{-X, -Y, -Z, W};
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("X=%.9f Y=%.9f Z=%.9f W=%.9f"), 
            static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z), static_cast<float>(W));
    }

    bool IsIdentity() const
    {
        return X == T(0) && Y == T(0) && Z == T(0) && W == T(1);
    }
};

template <typename T>
const TQuat<T> TQuat<T>::Identity = TQuat<T>(0, 0, 0, 1);

template <typename T>
inline FArchive& operator<<(FArchive& Ar, TQuat<T>& Q)
{
    return Ar << Q.X << Q.Y << Q.Z << Q.W;
}
