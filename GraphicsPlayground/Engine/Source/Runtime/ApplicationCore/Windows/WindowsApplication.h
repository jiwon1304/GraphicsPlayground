#pragma once

#include "ApplicationCore/Generic/GenericApplication.h"

struct FWindowsApplicationInitParams : public FGenericApplicationInitParams
{
    HINSTANCE hInstance;
};

class FWindowsApplication : public FGenericApplication
{
public:
    FWindowsApplication(FGenericApplicationInitParams* InInitParams)
        : FGenericApplication(InInitParams)
    {
        FWindowsApplicationInitParams* WindowsParams = static_cast<FWindowsApplicationInitParams*>(InInitParams);
        Instance = WindowsParams->hInstance;
    }

    virtual ~FWindowsApplication() = default;   

    virtual std::shared_ptr<FGenericWindow> MakeWindow() override;

private:
    HINSTANCE Instance;
};