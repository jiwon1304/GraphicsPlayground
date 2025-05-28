#include "VehicleMovementComponent.h"
#include "UObject/Casts.h"

UVehicleMovementComponent::UVehicleMovementComponent()
    : ChassisMass(100.0f), ChassisBoxExtents(FVector(2.5f, 2.0f, 5.0f))
    , WheelMass(20.0f), WheelRadius(0.5f), WheelWidth(0.4f)
    , NBWheels(4)
    , FrontLeftWheelOffset(FVector(-1.049995f, -1.5f, 1.5f))
    , FrontRightWheelOffset(FVector(1.049995f, -1.5f, 1.5f))
    , RearLeftWheelOffset(FVector(-1.049995f, -1.5f, -1.5f))
    , RearRightWheelOffset(FVector(1.049995f, -1.5f, -1.5f))
{
    // 기본적으로 Car의 SkeletalMesh를 활용해서 값 바꾸기 적용해줌

    ChassisCMOffset = FVector(0.0f, -ChassisBoxExtents.Y * 0.5f + 0.65f, 0.25f);
}

UObject* UVehicleMovementComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->ChassisMass = ChassisMass;
    NewComponent->ChassisBoxExtents = ChassisBoxExtents;
    NewComponent->WheelMass = WheelMass;
    NewComponent->WheelRadius = WheelRadius;
    NewComponent->WheelWidth = WheelWidth;
    NewComponent->NBWheels = NBWheels;
    NewComponent->FrontLeftWheelOffset = FrontLeftWheelOffset;
    NewComponent->FrontRightWheelOffset = FrontRightWheelOffset;
    NewComponent->RearLeftWheelOffset = RearLeftWheelOffset;
    NewComponent->RearRightWheelOffset = RearRightWheelOffset;

    NewComponent->ChassisCMOffset = ChassisCMOffset;

    return NewComponent;
}
