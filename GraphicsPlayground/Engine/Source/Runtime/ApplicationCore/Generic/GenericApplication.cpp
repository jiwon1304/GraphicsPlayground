#include "GenericApplication.h"

#include <chrono>
#include <thread>

void FGenericApplication::YieldThisThread()
{
    std::this_thread::yield();
}

void FGenericApplication::SleepFor(uint64 Microseconds)
{
    std::this_thread::sleep_for(std::chrono::microseconds(Microseconds));
}
