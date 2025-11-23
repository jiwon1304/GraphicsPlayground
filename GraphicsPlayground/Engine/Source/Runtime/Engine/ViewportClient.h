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
    virtual FMatrix GetProjectionMatrix() {}

    void DeprojectNDCToWorld(const FVector2D& InNDCPosition, FVector OutWorldOrigin, FVector& OutWorldDir);

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
    virtual FMatrix GetProjectionMatrix() override;

    float FOVDegree = 90.0f;
};

struct FViewportOrthographicCamera : public FViewportCamera
{
public:
    virtual FMatrix GetProjectionMatrix() override;

    float Width = 100.f;
};


class FViewportClient
{
public:
    virtual ~FViewportClient() = default;

    virtual UWorld* GetWorld() const { return nullptr; }

    void Draw(FViewport* Viewport);

    void Resize(const FIntRect& InRect);

    FViewportCamera* GetViewportCamera() const { return ViewportCamera; }
    virtual FViewportPerspectiveCamera* GetPerspectiveCamera() const { return nullptr; }
    virtual FViewportOrthographicCamera* GetOrthographicCamera() const { return nullptr; }

    FIntRect GetViewportRect() const { return ViewportRect; }
protected:
    FViewportCamera* ViewportCamera;

    // Where to draw in the application's window
    FIntRect ViewportRect;
};
