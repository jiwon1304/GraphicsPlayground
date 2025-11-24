#include "Math/JungleMath.h"
#include "MathUtility.h"

#include "Vector.h"
#include "Vector4.h"
#include "Rotator.h"
#include "Quat.h"
#include "Matrix.h"

FVector4 JungleMath::ConvertV3ToV4(const FVector& vec3)
{
    FVector4 newVec4;
    newVec4.X = vec3.X;
    newVec4.Y = vec3.Y;
    newVec4.Z = vec3.Z;
    return newVec4;
}

FMatrix JungleMath::CreateModelMatrix(const FVector& translation, const FVector& rotation, const FVector& scale)
{
    FMatrix Translation = FMatrix::CreateTranslationMatrix(translation);

    FMatrix Rotation = FMatrix::CreateRotationMatrix(FRotator{rotation.Y, rotation.Z, rotation.X});
    //FMatrix Rotation = JungleMath::EulerToQuaternion(rotation).ToMatrix();

    FMatrix Scale = FMatrix::CreateScaleMatrix(scale);
    return Scale * Rotation * Translation;
}

FMatrix JungleMath::CreateModelMatrix(const FVector& translation, const FQuat& rotation, const FVector& scale)
{
    FMatrix Translation = FMatrix::CreateTranslationMatrix(translation);
    FMatrix Rotation = rotation.ToMatrix();
    FMatrix Scale = FMatrix::CreateScaleMatrix(scale);
    return Scale * Rotation * Translation;
}
FMatrix JungleMath::CreateViewMatrix(const FVector& eye, const FVector& target, const FVector& PseudoUp)
{
    FVector zAxis = (target - eye).GetSafeNormal();  // DirectX는 LH이므로 -z가 아니라 +z 사용
    FVector xAxis = (PseudoUp.Cross(zAxis)).GetSafeNormal();
    FVector yAxis = zAxis.Cross(xAxis);

    FMatrix View;
    View.M[0][0] = xAxis.X; View.M[0][1] = yAxis.X; View.M[0][2] = zAxis.X; View.M[0][3] = 0;
    View.M[1][0] = xAxis.Y; View.M[1][1] = yAxis.Y; View.M[1][2] = zAxis.Y; View.M[1][3] = 0;
    View.M[2][0] = xAxis.Z; View.M[2][1] = yAxis.Z; View.M[2][2] = zAxis.Z; View.M[2][3] = 0;
    View.M[3][0] = -xAxis.Dot(eye);
    View.M[3][1] = -yAxis.Dot(eye);
    View.M[3][2] = -zAxis.Dot(eye);
    View.M[3][3] = 1;

    return View;
}

FMatrix JungleMath::CreateProjectionMatrix(const float fov, const float aspect, const float nearPlane, const float farPlane)
{
    float tanHalfFOV = tan(fov / 2.0f);
    float depth = farPlane - nearPlane;

    FMatrix Projection = {};
    Projection.M[0][0] = 1.0f / (aspect * tanHalfFOV);
    Projection.M[1][1] = 1.0f / tanHalfFOV;
    Projection.M[2][2] = farPlane / depth;
    Projection.M[2][3] = 1.0f;
    Projection.M[3][2] = -(nearPlane * farPlane) / depth;
    Projection.M[3][3] = 0.0f;  

    return Projection;
}

FMatrix JungleMath::CreateOrthoProjectionMatrix(const float width, const float height, const float nearPlane, const float farPlane)
{
    float r = width * 0.5f;
    float t = height * 0.5f;
    float invDepth = 1.0f / (farPlane - nearPlane);

    FMatrix Projection = {};
    Projection.M[0][0] = 1.0f / r;
    Projection.M[1][1] = 1.0f / t;
    Projection.M[2][2] = invDepth;
    Projection.M[3][2] = -nearPlane * invDepth;
    Projection.M[3][3] = 1.0f;

    return Projection;
}

/**
 * Create an off-center orthographic projection.
 * @param left     좌표계의 왼쪽(Xmin)
 * @param right    좌표계의 오른쪽(Xmax)
 * @param bottom   좌표계의 아래(Ymin)
 * @param top      좌표계의 위(Ymax)
 * @param nearPlane  near plane 거리
 * @param farPlane   far plane 거리
 */
FMatrix JungleMath::CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top,
    const float nearPlane, const float farPlane)
{
    float width = right - left;
    float height = top - bottom;
    float invWidth = 2.0f / width;       // = 1 / (width*0.5f)
    float invHeight = 2.0f / height;      // = 1 / (height*0.5f)
    float invDepth = 1.0f / (farPlane - nearPlane);

    FMatrix Projection = {};

    Projection.M[0][0] = invWidth;            // X 스케일
    Projection.M[1][1] = invHeight;           // Y 스케일
    Projection.M[2][2] = invDepth;            // Z 스케일 (0~1)
    Projection.M[3][0] = -(right + left) / width;   // X 오프셋
    Projection.M[3][1] = -(top + bottom) / height; // Y 오프셋
    Projection.M[3][2] = -nearPlane * invDepth;      // Z 오프셋
    Projection.M[3][3] = 1.0f;

    return Projection;
}

FVector JungleMath::FVectorRotate(const FVector& origin, const FVector& InRotation)
{
    FQuat quaternion = JungleMath::EulerToQuaternion(InRotation);
    // 쿼터니언을 이용해 벡터 회전 적용
    return quaternion.RotateVector(origin);
}
FQuat JungleMath::EulerToQuaternion(const FVector& eulerDegrees)
{
    float yaw = FMath::DegreesToRadians(eulerDegrees.Z);   // Z축 Yaw
    float pitch = FMath::DegreesToRadians(eulerDegrees.Y); // Y축 Pitch
    float roll = FMath::DegreesToRadians(eulerDegrees.X);  // X축 Roll

    float halfYaw = yaw * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfRoll = roll * 0.5f;

    float cosYaw = cos(halfYaw);
    float sinYaw = sin(halfYaw);
    float cosPitch = cos(halfPitch);
    float sinPitch = sin(halfPitch);
    float cosRoll = cos(halfRoll);
    float sinRoll = sin(halfRoll);

    FQuat quat;
    quat.W = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;
    quat.X = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
    quat.Y = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
    quat.Z = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;

    quat.Normalize();
    return quat;
}
FVector JungleMath::QuaternionToEuler(const FQuat& quat)
{
    FVector euler;

    // 쿼터니언 정규화
    FQuat q = quat;
    q.Normalize();

    // Yaw (Z 축 회전)
    float sinYaw = 2.0f * (q.W * q.Z + q.X * q.Y);
    float cosYaw = 1.0f - 2.0f * (q.Y * q.Y + q.Z * q.Z);
    euler.Z = FMath::RadiansToDegrees(atan2(sinYaw, cosYaw));

    // Pitch (Y 축 회전, 짐벌락 방지)
    float sinPitch = 2.0f * (q.W * q.Y - q.Z * q.X);
    if (fabs(sinPitch) >= 1.0f)
    {
        euler.Y = FMath::RadiansToDegrees(copysign(PI / 2, sinPitch)); // 🔥 Gimbal Lock 방지
    }
    else
    {
        euler.Y = FMath::RadiansToDegrees(asin(sinPitch));
    }

    // Roll (X 축 회전)
    float sinRoll = 2.0f * (q.W * q.X + q.Y * q.Z);
    float cosRoll = 1.0f - 2.0f * (q.X * q.X + q.Y * q.Y);
    euler.X = FMath::RadiansToDegrees(atan2(sinRoll, cosRoll));
    return euler;
}

FVector JungleMath::FVectorRotate(const FVector& origin, const FRotator& InRotation)
{
    return InRotation.Quaternion().RotateVector(origin);
}

FVector JungleMath::FVectorRotate(const FVector& origin, const FQuat& InRotation)
{
    return InRotation.RotateVector(origin);
}
