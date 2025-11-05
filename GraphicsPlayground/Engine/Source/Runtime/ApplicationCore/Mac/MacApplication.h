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



ㅇㄹㅁㅇㄹㅁ리사이즈 필요없다!!! 콜백만받고 함수로 진짜 크기조절하는건 필요없음
그냥 뷰포트만 바꾸면된다ㅏ