#pragma once
#include "Axis.h"
#include "MathFwd.h"
#include "MathUtility.h"
#include "Serialization/Archive.h"

// 4x4 행렬 연산
template <typename T>
struct alignas(16) TMatrix
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");
public:
    alignas(16) T M[4][4];

    using FReal = T;

public:
    static const TMatrix<T> Identity;

public:
    // 기본 연산자 오버로딩
    TMatrix operator+(const TMatrix<T>& Other) const;
    TMatrix operator-(const TMatrix<T>& Other) const;
    TMatrix operator*(const TMatrix<T>& Other) const;
    TMatrix& operator*=(const TMatrix<T>& Other);
    TMatrix operator*(T Scalar) const;
    TMatrix operator/(T Scalar) const;
    T* operator[](int row);
    const T* operator[](int row) const;

    TVector<T> ExtractScaling(T Tolerance = SMALL_NUMBER);
    TVector<T> GetOrigin() const;
    void SetOrigin(const TVector<T>& NewOrigin);
    TVector4<T> GetColumn(int32 ColumnIndex) const;
    T Determinant() const;

    void SetAxis(int32 i, const TVector<T>& Axis);
    TVector<T> GetScaledAxis(EAxis::Type InAxis) const;

    // 유틸리티 함수
    static TMatrix Transpose(const TMatrix<T>& Mat);
    static TMatrix Inverse(const TMatrix<T>& Mat);
    static TMatrix CreateTranslationMatrix(const TVector<T>& V);
    static TMatrix CreateRotationMatrix(const TRotator<T>& R);
    static TMatrix CreateRotationMatrix(const TQuat<T>& Q);
    static TMatrix CreateScaleMatrix(const TVector<T>& V);

    static TVector<T> TransformVector(const TVector<T>& V, const TMatrix<T>& M);
    static TVector4<T> TransformVector(const TVector4<T>& V, const TMatrix<T>& M);

    TVector4<T> TransformVector4(const TVector4<T>& vector) const;
    TVector<T> TransformPosition(const TVector<T>& vector) const;

    TQuat<T> ToQuat() const;

    TVector<T> GetScaleVector(T Tolerance = SMALL_NUMBER) const;

    TVector<T> GetTranslationVector() const;

    TMatrix GetMatrixWithoutScale(T Tolerance = SMALL_NUMBER) const;

    TVector4<T> TransformVector(const TVector<T>& V) const;

    /**
     *	이 매트릭스의 역행렬로 방향 벡터를 변환합니다. (위치 정보는 무시됨)
     *	표면 노멀(또는 평면)을 변환하고 비균일 스케일링까지 올바르게 적용하려면
     *	행렬 역행렬의 adjoint를 사용하는 TransformByUsingAdjointT를 사용하세요.
     */
    TVector<T> InverseTransformVector(const TVector<T>& V) const;

    void RemoveScaling(T Tolerance = SMALL_NUMBER);

    bool Equals(const TMatrix& Other, T Tolerance = KINDA_SMALL_NUMBER) const;
};

template <typename T>
inline FArchive& operator<<(FArchive& Ar, TMatrix<T>& M)
{
    Ar << M.M[0][0] << M.M[0][1] << M.M[0][2] << M.M[0][3];
    Ar << M.M[1][0] << M.M[1][1] << M.M[1][2] << M.M[1][3];
    Ar << M.M[2][0] << M.M[2][1] << M.M[2][2] << M.M[2][3];
    Ar << M.M[3][0] << M.M[3][1] << M.M[3][2] << M.M[3][3];
    return Ar;
}
