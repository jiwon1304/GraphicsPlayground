# --------------------------------------------------------------------------------------------------
# (최상위 CMakeLists.txt)
# - 64비트 전용
# - C++20 + (새 요구) C 소스도 1개 이상 컴파일해야 하므로 C 언어 활성화
# - 실제 타겟 설정은 GraphicsPlayground/CMakeLists.txt 에서 수행
# --------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 다중 OS에서 동일한 빌드 디렉터리를 사용할 때, 이전에 Windows 전용 설정이 캐시에 남아있을 수 있음.
# Unix Makefiles/Ninja는 generator platform 개념을 지원하지 않기 때문에, non-Windows 환경에서는 해당 값을 비워준다.
if (NOT WIN32)
    if (DEFINED CMAKE_GENERATOR_PLATFORM AND NOT "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "")
        message(STATUS "Clearing cached CMAKE_GENERATOR_PLATFORM='${CMAKE_GENERATOR_PLATFORM}' for generator '${CMAKE_GENERATOR}'")
        unset(CMAKE_GENERATOR_PLATFORM CACHE)
    endif()
    if (DEFINED CMAKE_GENERATOR_TOOLSET AND NOT "${CMAKE_GENERATOR_TOOLSET}" STREQUAL "")
        message(STATUS "Clearing cached CMAKE_GENERATOR_TOOLSET='${CMAKE_GENERATOR_TOOLSET}' for generator '${CMAKE_GENERATOR}'")
        unset(CMAKE_GENERATOR_TOOLSET CACHE)
    endif()
endif()

# 프로젝트 정의: LANGUAGES 에 C 추가 (ThirdParty C 파일을 컴파일하기 위함)
project(GraphicsPlaygroundProj LANGUAGES C CXX)

# 구성 타입(Visual Studio 등 멀티-컨피그 제너레이터용)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# C++ 표준 (요구: C++23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# (선택) C 표준: ThirdParty C 코드가 특정 표준 요구 없으면 C11 정도 권장
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# MSVC 런타임 라이브러리(/MD, /MDd) 설정
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# 64비트 전용 빌드 확인
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "x64 (64-bit) 빌드를 진행합니다.")
else()
    message(FATAL_ERROR "이 프로젝트는 64비트(x64)만 지원합니다.")
endif()

# 추후 유틸/전역 설정이 있으면 include(...)
# include(CMake/Utils.cmake OPTIONAL)

# 하위 디렉터리 추가: 실제 타겟 정의
add_subdirectory(GraphicsPlayground)