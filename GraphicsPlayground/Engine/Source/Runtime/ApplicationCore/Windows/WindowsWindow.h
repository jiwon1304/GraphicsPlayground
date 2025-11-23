#include "ApplicationCore/Generic/GenericWindow.h"

struct FWindowsWindowInitParams : public FGenericWindowInitParams
{
    HINSTANCE hInstance;
};

class FWindowsWindow : public FGenericWindow
{
    friend class FWindowsApplication;

public:
    // virtual void ReshapeWindow(int32 X, int32 Y, int32 Width, int32 Height) override;

    virtual void GetWindowShape(int32& X, int32& Y, int32& Width, int32& Height) const override;

    virtual bool IsActive() const override;

    HWND WindowHandle;

protected:
    FWindowsWindow(std::shared_ptr<FGenericWindowInitParams> InParams);
};
