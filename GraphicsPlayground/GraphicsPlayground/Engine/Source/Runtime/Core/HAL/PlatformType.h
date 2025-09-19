#pragma once
#include <cstdint>
#include <type_traits>

//-----------------------------------------
// OS별 분기
//-----------------------------------------
#if defined(_WIN32) || defined(_WIN64)
#define BUILD_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define BUILD_PLATFORM_MAC
#else
static_assert(false, "Unsupported platform");
#endif


// ---------------- Windows ----------------
#ifdef BUILD_PLATFORM_WINDOWS
#define _TCHAR_DEFINED
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef max
#undef min

#ifdef TEXT
    #undef TEXT
#endif

// inline 강제
#define FORCEINLINE __forceinline
#define FORCENOINLINE __declspec(noinline)
#define RESTRICT __restrict
#endif


// ---------------- macOS / Linux ----------------
#ifdef BUILD_PLATFORM_MAC
#include <unistd.h>   // 필요시 POSIX API
#include <sys/types.h>

// Windows 전용 키워드 대체
#define FORCEINLINE inline __attribute__((always_inline))
#define FORCENOINLINE __attribute__((noinline))
#define RESTRICT __restrict__

#endif
//-----------------------------------------

// Debug 모드에서만 inline 강제 해제
#ifdef _DEBUG
    #define FORCEINLINE_DEBUGGABLE inline
#else
    #define FORCEINLINE_DEBUGGABLE FORCEINLINE
#endif

//-----------------------------------------
// 문자열 관련 매크로
//-----------------------------------------
#define USE_WIDECHAR 0

#if USE_WIDECHAR 
    #define TEXT(x) L##x
#else
    #define TEXT(x) x
#endif

//-----------------------------------------
// typedef
//-----------------------------------------
typedef std::uint8_t  uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

typedef std::int8_t   int8;
typedef std::int16_t  int16;
typedef std::int32_t  int32;
typedef std::int64_t  int64;

typedef char     ANSICHAR;
typedef wchar_t  WIDECHAR;

#if USE_WIDECHAR
typedef WIDECHAR TCHAR;
#else
typedef ANSICHAR TCHAR;
#endif

//-----------------------------------------
// 임시로 사용
//-----------------------------------------
#include <string>
#if USE_WIDECHAR
[[deprecated("Use FString instead")]]
#endif
typedef std::wstring FWString;
