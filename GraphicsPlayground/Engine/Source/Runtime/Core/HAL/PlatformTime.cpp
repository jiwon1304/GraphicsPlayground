#include "PlatformTime.h"
#include "Windows/WindowsPlatformTime.h"
#include "Unix/UnixPlatformTime.h"

FPlatformTime* GPlatformTime = nullptr;
double FPlatformTime::SecondsPerCycle = 0.0;
bool FPlatformTime::bInitialized = false;

void FPlatformTime::InitTiming()
{
    GPlatformTime = FPlatformTime::CreatePlatformTime();
    GPlatformTime->InitTiming_Internal();
}

double FPlatformTime::GetSecondsPerCycle()
{
    if (!bInitialized)
    {
        InitTiming();
    }
    return SecondsPerCycle;
}

double FPlatformTime::ToMilliseconds(uint64 CycleDiff)
{
    const double Ms = static_cast<double>(CycleDiff)
        * GPlatformTime->GetSecondsPerCycle()
        * 1000.0;

    return Ms;
}

uint64 FPlatformTime::Cycles64()
{
    return GPlatformTime->Cycles_Internal();
}

uint32 FPlatformTime::Cycles32()
{
    return static_cast<uint32>(Cycles64());
}

FPlatformTime *FPlatformTime::CreatePlatformTime()
{
#ifdef BUILD_PLATFORM_WINDOWS
    return new FWindowsPlatformTime();
#else
    return new FUnixPlatformTime();
#endif
}
