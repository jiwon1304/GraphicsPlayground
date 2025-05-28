#pragma once

#include "Engine/Classes/Components/ActorComponent.h"

class UVehicleMovementComponent : public UActorComponent 
{
    DECLARE_CLASS(UVehicleMovementComponent, UActorComponent)

public:
    UVehicleMovementComponent();

    virtual UObject* Duplicate(UObject* InOuter) override;

    // UVehicleMovement는 데이터만 Hold
    // RegisterObject와 Fetch 부분은 Vehicle4W 부분에서 처리

    // --- Vehicle 파라미터 ---
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, ChassisMass // 차체 질량 (kg)
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, ChassisBoxExtents // 차체 박스 Extents (cm) 예제는 값이 작긴함
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, ChassisCMOffset // 질량 중심 오프셋, 로컬 좌표계의 원점과 얼마나 떨어졌나
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, PeakTorque
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, MaxOmega
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, ClutchStrength
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, WheelMass // 바퀴 질량 (kg)
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, WheelRadius // 바퀴 반지름 (m)
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        float, WheelWidth // 바퀴 높이 (m)
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        UINT32, NBWheels // 바퀴 갯수 (m)
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, FrontLeftWheelOffset
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, FrontRightWheelOffset
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, RearLeftWheelOffset
    )

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FVector, RearRightWheelOffset
    )
};
