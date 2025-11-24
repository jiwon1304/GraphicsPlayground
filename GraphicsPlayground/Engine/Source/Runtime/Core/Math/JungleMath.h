#pragma once
#include "MathFwd.h"

//  Near Clip Plane 값을 정의한 헤더
#ifndef NEAR_PLANE
#define NEAR_PLANE 1.f 
#endif

class JungleMath
{
public:
    static FVector4 ConvertV3ToV4(const FVector& vec3);
    static FMatrix CreateModelMatrix(const FVector& translation, const FVector& rotation, const FVector& scale);
    static FMatrix CreateModelMatrix(const FVector& translation, const FQuat& rotation, const FVector& scale);
    static FMatrix CreateViewMatrix(const FVector& eye, const FVector& target, const FVector& PseudoUp);
    static FMatrix CreateProjectionMatrix(const float fov, const float aspect, const float nearPlane, const float farPlane);
    static FMatrix CreateOrthoProjectionMatrix(const float width, const float height, const float nearPlane, const float farPlane);
    static FMatrix CreateOrthographicOffCenter(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane);

    static FVector FVectorRotate(const FVector& origin, const FVector& InRotation);
    static FVector FVectorRotate(const FVector& origin, const FRotator& InRotation);
    static FVector FVectorRotate(const FVector& origin, const FQuat& InRotation);
    static FQuat EulerToQuaternion(const FVector& eulerDegrees);
    static FVector QuaternionToEuler(const FQuat& quat);
};
