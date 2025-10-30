#include "HAL/PlatformTime.h"

#ifdef BUILD_PLATFORM_MACOS
class FUnixPlatformTime : public FPlatformTime
{
protected:
    virtual void InitTiming_Internal() override;

    virtual uint64 Cycles_Internal() override;
};
#endif