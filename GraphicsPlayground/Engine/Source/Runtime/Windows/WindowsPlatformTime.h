#pragma once
#include "HAL/PlatformTime.h"

#ifdef BUILD_PLATFORM_WINDOWS
/**
 * Windows 플랫폼에서의 시간 관련 기능을 제공하는 클래스
 */
class FWindowsPlatformTime : public FPlatformTime
{
protected:
    virtual void InitTiming_Internal() override;
    
    virtual uint64 Cycles_Internal() override;
    
private:
    /**
     * CPU 주파수를 반환하는 함수
     * @return uint64 CPU 주파수
     */
    virtual uint64 GetFrequency() override;
};
#endif