#include "ApplicationCore/Generic/GenericWindow.h"

class FWindowsWindow : public FGenericWindow
{
    friend class FWindowsApplication;

public:
    FWindowsWindow();

    virtual void ReshapeWindow(int32 X, int32 Y, int32 Width, int32 Height) override;

    virtual void GetWindowShape(int32& X, int32& Y, int32& Width, int32& Height) const override;

    virtual void Activate() override;

    virtual bool IsActive() const override;

private:
    HWND WindowHandle;

    void Initialize()
};