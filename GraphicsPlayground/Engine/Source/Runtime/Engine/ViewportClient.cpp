#include "ViewportClient.h"

#include "Core/Math/JungleMath.h"
#include "Core/Math/Vector2.h"

void FViewportCamera::MoveForward(float InValue)
{
    ViewLocation = ViewLocation + GetForwardVector() * InValue;
}

void FViewportCamera::MoveRight(float InValue)
{
    ViewLocation = ViewLocation + GetRightVector() * InValue;
}

void FViewportCamera::MoveUp(float InValue)
{
    ViewLocation = ViewLocation + GetUpVector() * InValue;
}

void FViewportCamera::RotateYaw(float InValue)
{
    ViewRotation.Yaw += InValue;
}

void FViewportCamera::RotatePitch(float InValue)
{
    ViewRotation.Pitch = FMath::Clamp(ViewRotation.Pitch + InValue, -89.f, 89.f);
}

FVector FViewportCamera::GetForwardVector() const
{
    return JungleMath::FVectorRotate(FVector::ForwardVector, ViewRotation);
}

FVector FViewportCamera::GetRightVector() const
{
    return JungleMath::FVectorRotate(FVector::RightVector, ViewRotation);
}

FVector FViewportCamera::GetUpVector() const
{
    return JungleMath::FVectorRotate(FVector::UpVector, ViewRotation);
}

FMatrix FViewportCamera::GetViewMatrix() const
{
    return JungleMath::CreateViewMatrix(
        ViewLocation,
        ViewLocation + GetForwardVector(),
        PseudoUp
    );
}

void FViewportCamera::DeprojectNDCToWorld(const FVector2D& InNDCPosition, FVector OutWorldOrigin, FVector& OutWorldDir)
{
    FVector RayOrigin = { InNDCPosition.X, InNDCPosition.Y, 0.0f };
    FVector RayEnd = { InNDCPosition.X, InNDCPosition.Y, 1.0f };

    // 스크린 좌표계에서 월드 좌표계로 변환
    const FMatrix ViewProj = GetViewMatrix() * GetProjectionMatrix();
    const FMatrix ViewProjInv = FMatrix::Inverse(ViewProj);
    RayOrigin = ViewProjInv.TransformPosition(RayOrigin);
    RayEnd = ViewProjInv.TransformPosition(RayEnd);

    OutWorldOrigin = RayOrigin;
    OutWorldDir = (RayEnd - RayOrigin).GetSafeNormal();
}

FMatrix FViewportPerspectiveCamera::GetProjectionMatrix()
{
    return JungleMath::CreateProjectionMatrix(
        FOVDegree,
        AspectRatio,
        NearClip,
        FarClip
    );
}

FMatrix FViewportOrthographicCamera::GetProjectionMatrix()
{
    return JungleMath::CreateOrthoProjectionMatrix(
        Width,
        Width / AspectRatio,
        NearClip,
        FarClip
    );
}

void FViewportClient::Resize(const FIntRect& InRect)
{
    ViewportRect = InRect;
    ViewportCamera->AspectRatio = static_cast<float>(ViewportRect.GetWidth()) / static_cast<float>(ViewportRect.GetHeight());
}
