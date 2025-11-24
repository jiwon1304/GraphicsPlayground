#include "Windows/WindowsPlatformTime.h"

void FWindowsPlatformTime::InitTiming_Internal()
{
    if (!bInitialized)
    {
        bInitialized = true;

        double Frequency = static_cast<double>(GetFrequency());
        if (Frequency <= 0.0)
        {
            Frequency = 1.0;
        }

        SecondsPerCycle = 1.0 / Frequency;
    }
}

uint64 FWindowsPlatformTime::GetFrequency()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    return Frequency.QuadPart;
}

uint64 FWindowsPlatformTime::Cycles_Internal()
{
    LARGE_INTEGER CycleCount;
    QueryPerformanceCounter(&CycleCount);
    return static_cast<uint64>(CycleCount.QuadPart);
}