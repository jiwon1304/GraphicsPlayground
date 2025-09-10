#pragma once

#include "Core/HAL/PlatformType.h"

/**
 * Aligns the value to the nearest higher multiple of "Alignment".
 * Must be a power of two.
 */
template <typename T>
FORCEINLINE constexpr T Align(T Value, size_t Alignment)
{
    static_assert(std::is_integral_v<T>, "Align can only be used with integral types");
    static_assert(Alignment && (Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two");
    return (Value + static_cast<T>(Alignment - 1)) & static_cast<T>(~(Alignment - 1));
}

/**
 * Aligns the value to the nearest lower multiple of "Alignment".
 * Must be a power of two.
 */
template <typename T>
FORCEINLINE constexpr T AlignDown(T Value, size_t Alignment)
{
    static_assert(std::is_integral_v<T>, "AlignDown can only be used with integral types");
    static_assert(Alignment && (Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two");
    return Value & static_cast<T>(~(Alignment - 1));
}