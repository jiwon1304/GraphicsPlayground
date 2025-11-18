#pragma once

#include "Core/Math/MathFwd.h"
#include "Core/Math/Rect.h"
#include "Core/Delegates/DelegateCombination.h"

DECLARE_DELEGATE_TwoParams(FOnWindowResizedDelegate, int32 /* NewWidth */, int32 /* NewHeight */);

struct FGenericWindowInitParams
{
    const char* Title = "GraphicsPlayground";
    FRect InitialWindowRect;
};

/**
 * Simple wrapper for window
 * This does not equal to viewport; 
 * You have to change viewport separately after resizing window
 */
class FGenericWindow
{
    friend class FGenericApplication;

public:
    // virtual void ReshapeWindow(int32 X, int32 Y, int32 Width, int32 Height) = 0;

    virtual void GetWindowShape(int32& X, int32& Y, int32& Width, int32& Height) const = 0;

    // /**
    //  * Register this to callback
    //  */
    // void SetActive(bool bActivate) { bActive = bActivate; }

    virtual bool IsActive() const = 0;

    FOnWindowResizedDelegate OnWindowResized;

protected:
    FGenericWindow(std::shared_ptr<FGenericWindowInitParams> Params) : CurrentWindowRect(Params->InitialWindowRect) {}
    virtual ~FGenericWindow() = default;

    FRect CurrentWindowRect;

    /**
     * Called by the platform-specific window when resized
     */
    void OnResize(int32 NewWidth, int32 NewHeight)
    {
        CurrentWindowRect = FRect({0, 0}, {static_cast<float>(NewWidth), static_cast<float>(NewHeight)});
        OnWindowResized.Execute(NewWidth, NewHeight);
    }

    // bool bActive = false;
};