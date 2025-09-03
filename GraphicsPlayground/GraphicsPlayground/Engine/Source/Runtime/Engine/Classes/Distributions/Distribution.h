#pragma once
#include "Math/CurveEdInterface.h"
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"


class UDistribution : public UObject, public FCurveEdInterface
{
    DECLARE_CLASS(UDistribution, UObject)

public:
    UDistribution() = default;
    virtual ~UDistribution() override = default;

    static const float DefaultValue;
};
