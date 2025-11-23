#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Rotator.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Rect.h"

class FViewport;
class UWorld;

constexpr float MIN_ORTHOZOOM = 1.f;
constexpr float MAX_ORTHOZOOM = static_cast<float>(uint32(1) << 30);

struct FViewportCamera
{
public:
    FViewportCamera(FVector InLocation, FRotator InRotation, float InAspectRatio)
        : ViewLocation(InLocation)
        , ViewRotation(InRotation)
        , AspectRatio(InAspectRatio)
    {
    }

    void MoveForward(float InValue);
    void MoveRight(float InValue);
    void MoveUp(float InValue);
    void RotateYaw(float InValue);
    void RotatePitch(float InValue);

    FVector GetForwardVector() const;
    FVector GetRightVector() const;
    FVector GetUpVector() const;

    FMatrix GetViewMatrix() const;
    virtual FMatrix GetProjectionMatrix() = 0;

    void DeprojectNDCToWorld(const FVector2D& InNDCPosition, FVector OutWorldOrigin, FVector& OutWorldDir);
    void DeprojectNDCToView(const FVector2D& InNDCPosition, FVector OutViewOrigin, FVector& OutViewDir);

    FVector ViewLocation;
    FRotator ViewRotation;
    FVector PseudoUp = FVector::UpVector;

    float AspectRatio;

    float NearClip = 0.1f;
    float FarClip = 1000.0f;
};

struct FViewportPerspectiveCamera : public FViewportCamera
{
public:
    FViewportPerspectiveCamera(FVector InLocation, FRotator InRotation, float InAspectRatio)
        : FViewportCamera(InLocation, InRotation, InAspectRatio)
    {
    }

    virtual FMatrix GetProjectionMatrix() override;

    float FOVDegree = 90.0f;
};

struct FViewportOrthographicCamera : public FViewportCamera
{
public:
    FViewportOrthographicCamera(FVector InLocation, FRotator InRotation, float InAspectRatio)
        : FViewportCamera(InLocation, InRotation, InAspectRatio)
    {
    }

    virtual FMatrix GetProjectionMatrix() override;

    float Width = 100.f;
};


class FViewportClient
{
public:
    virtual ~FViewportClient();

    virtual void Initialize(const FIntRect& InRect, FViewportCamera* InCamera);

    virtual UWorld* GetWorld() const { return nullptr; }

    void Draw(FViewport* Viewport);

    void Resize(const FIntRect& InRect);

    FViewportCamera* GetViewportCamera() const { return ViewportCamera; }

    FIntRect GetViewportRect() const { return ViewportRect; }
protected:
    FViewportCamera* ViewportCamera;

    // Where to draw in the application's window
    FIntRect ViewportRect;
};
