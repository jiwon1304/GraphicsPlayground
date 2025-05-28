#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"

namespace ECameraProjectionMode
{
    enum Type : int
    {
        Perspective,
        Orthographic
    };
}

struct FPostProcessSettings
{
    DECLARE_STRUCT(FPostProcessSettings)

    UPROPERTY(
        EditAnywhere,
        bool,
        bDepthOfField,
        = false
    )

    // 초점이 맞기 시작하는 거리
    UPROPERTY(
        EditAnywhere,
        float,
        DepthOfFieldFocalDistance,
        = 0.f
        )
    // DepthOfFieldFocalDistance에서부터 뒤로 초점이 맞는 거리
    UPROPERTY(
        EditAnywhere,
        float,
        DepthOfFieldFocalRegion,
        = 0.f
    )
    // 거리에 따른 흐림 정도
    UPROPERTY(
        EditAnywhere,
        float,
        DepthOfFieldDepthBlurAmount,
        = 0.f
    )
    // 1920x1080 기준으로 흐림 반경
    UPROPERTY(
        EditAnywhere,
        float,
        DepthOfFieldDepthBlurRadius,
        = 0.f
    )


    
};

/* 카메라 필요 인자 추가하시면 됩니다 */
struct FMinimalViewInfo
{
    FVector Location;
    FRotator Rotation;
    float FOV;
    float PerspectiveNearClip;
    float PerspectiveFarClip;

    /*float OrthoWidth;
    float OrthoNearClipPlane;
    float OrthoFarClipPlane;
    float AspectRatio;*/
    FMinimalViewInfo()
        : Location(0.0f, 0.0f, 0.0f)
        , Rotation(0.0f, 0.0f, 0.0f)
        , FOV(90.0f)
        , PerspectiveNearClip(0.1f)
        , PerspectiveFarClip(1000.f)
        /*, OrthoWidth(512.0f)
        , OrthoNearClipPlane(1.0f)
        , OrthoFarClipPlane(10000.0f)
        , AspectRatio(1.33333333f)*/
    {
    }

    bool Equals(const FMinimalViewInfo& Other) const;
    void BlendViewInfo(FMinimalViewInfo& OtherInfo, float OtherWeight);
};

inline bool FMinimalViewInfo::Equals(const FMinimalViewInfo& Other) const
{
    return 
        Location == Other.Location &&
        Rotation == Other.Rotation &&
        FOV == Other.FOV &&
        PerspectiveNearClip == Other.PerspectiveNearClip &&
        PerspectiveFarClip == Other.PerspectiveFarClip;
}

inline void FMinimalViewInfo::BlendViewInfo(FMinimalViewInfo& OtherInfo, float OtherWeight)
{
    Location = FMath::Lerp(Location, OtherInfo.Location, OtherWeight);

    const FRotator DeltaAng = (OtherInfo.Rotation - Rotation).GetNormalized();
    Rotation = Rotation + OtherWeight * DeltaAng;

    FOV = FMath::Lerp(FOV, OtherInfo.FOV, OtherWeight);
}

