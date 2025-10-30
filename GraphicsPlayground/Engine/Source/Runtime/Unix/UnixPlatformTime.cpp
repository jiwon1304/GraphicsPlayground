#include "UnixPlatformTime.h"

#ifdef BUILD_PLATFORM_MACOS
void FUnixPlatformTime::InitTiming_Internal()
{
    if (bInitialized) return;

#if defined(CLOCK_MONOTONIC)
    // If CLOCK_MONOTONIC exists, use nanoseconds resolution
    SecondsPerCycle = 1e-9; // Cycles returned in nanoseconds
#else
    // Fallback to gettimeofday -> microseconds
    SecondsPerCycle = 1e-6;
#endif

    bInitialized = true;
}

uint64 FUnixPlatformTime::Cycles_Internal()
{
#if defined(CLOCK_MONOTONIC)
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64>(ts.tv_sec) * 1000000000ull + static_cast<uint64>(ts.tv_nsec); // ns
#else
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return static_cast<uint64>(tv.tv_sec) * 1000000ull + static_cast<uint64>(tv.tv_usec); // us
#endif
}
#endif