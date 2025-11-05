#pragma once

#include "ApplicationCore/Generic/GenericWindow.h"
#include "OpenGLDrv/OpenGLFwd.h"

struct FMacWindowInitParams : public FGenericWindowInitParams
{
    // From PlatformCreateOpenGLDevice()
    FOpenGL::Window* WindowHandle;
};

class FMacWindow : public FGenericWindow
{
    friend class FMacApplication;

public:
    virtual void ReshapeWindow(int32 X, int32 Y, int32 Width, int32 Height) override;

    virtual void GetWindowShape(int32& X, int32& Y, int32& Width, int32& Height) const override;

    // virtual void Activate() override;

    virtual bool IsActive() const override;

protected:
    FOpenGL::Window* WindowHandle;

    FMacWindow(std::shared_ptr<FGenericWindowInitParams> Params);
};