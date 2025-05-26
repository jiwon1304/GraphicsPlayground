#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"

// FChaosPhysicsMaterial

// FChaosPhysicsMaterial을 기준으로 작성
struct FPhysicsMaterial
{
    DECLARE_STRUCT(FPhysicsMaterial)

    // 물리 엔진에서 사용
    struct FPhysxUserData* UserData = nullptr;

    // 물리 재질의 밀도
    UPROPERTY(
        EditAnywhere,
        float,
        Density,
        = 1.0f
    )
    // 마찰 계수
    UPROPERTY(
        EditAnywhere,
        float,
        Friction,
        = 0.5f
    )
    // 정지 마찰 계수
    UPROPERTY(
        EditAnywhere,
        float,
        StaticFriction,
        = 0.5f
    )
    // 벽에 충돌했을 때 튕기는 정도. 0일경우 튕기지 않음. 1일 경우 충돌 시 동일한 속도로 튕김
    UPROPERTY(
        EditAnywhere,
        float,
        Restitution,
        = 0.5f
    )
};
