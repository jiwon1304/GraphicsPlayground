#pragma once
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"

class USkinnedAsset : public UObject
{
    DECLARE_CLASS(USkinnedAsset, UObject)

public:
    USkinnedAsset() = default;
    virtual ~USkinnedAsset() override = default;
};
