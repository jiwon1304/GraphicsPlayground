#pragma once

#include "ApplicationCore/Generic/GenericApplication.h"

struct FMacApplicationInitParams : public FGenericApplicationInitParams
{
};

/**
 * GLFW does not have explicit application handle like Windows HINSTANCE
 * So this class is mostly empty and just for platform abstraction
 */
class FMacApplication : public FGenericApplication
{
public:
    FMacApplication(FGenericApplicationInitParams* InInitParams) : FGenericApplication(InInitParams) {}
    virtual ~FMacApplication() = default;

    virtual std::shared_ptr<FGenericWindow> MakeWindow(std::shared_ptr<FGenericWindowInitParams> Params) override;
private:

};