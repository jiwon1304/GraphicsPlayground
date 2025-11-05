#pragma once

#include "Core/Math/MathFwd.h"
#include "Core/Math/Primitive.h"

struct FGenericWindowInitParams
{
    const char* Title = "GraphicsPlayground";
    FRect InitialWindowRect;
};

/**
 * Simple wrapper for window
 */
class FGenericWindow
{
    friend class FGenericApplication;

public:
    virtual void ReshapeWindow(int32 X, int32 Y, int32 Width, int32 Height) = 0;

    virtual void GetWindowShape(int32& X, int32& Y, int32& Width, int32& Height) const = 0;

    // /**
    //  * Register this to callback
    //  */
    // void SetActive(bool bActivate) { bActive = bActivate; }

    virtual bool IsActive() const = 0;

protected:
    FGenericWindow(std::shared_ptr<FGenericWindowInitParams> Params) : CurrentWindowRect(Params->InitialWindowRect) {}
    virtual ~FGenericWindow() = default;

    FRect CurrentWindowRect;

    // bool bActive = false;
};