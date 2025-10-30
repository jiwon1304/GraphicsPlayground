# --------------------------------------------------------------------------------------------------
# GraphicsPlayground/CMakeLists.txt
# - Windows: DirectX11 또는 OpenGL (둘 중 하나)
# - macOS  : OpenGL만
# --------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)

project(GraphicsPlayground LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_options(-Wno-invalid-offsetof) # for ObjectMacro's offsetof()
endif()
message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

# Graphics API Settings
option(API_FALLBACK "Enable API Fallback (force OpenGL)" OFF)
if(API_FALLBACK OR APPLE)
    set(USE_OPENGL ON)
    set(USE_DIRECTX11 OFF)
else()
    set(USE_DIRECTX11 ON)
    set(USE_OPENGL OFF)
endif()

# Project Settings
set(PROJECT_NAME GraphicsPlayground)
set(ROOT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/ThirdParty")
set(BUILD_TARGET_OS "${CMAKE_SYSTEM_NAME}")

if ( CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|x64)$" )
    set(BUILD_TARGET_ARCH "x64")
elseif ( CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|ARM64|arm64)$" )
    set(BUILD_TARGET_ARCH "arm64")
else()
    message(FATAL_ERROR "지원하지 않는 아키텍처: ${CMAKE_SYSTEM_PROCESSOR}")
    set(BUILD_TARGET_ARCH "x64")
endif()

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE이 설정되지 않았습니다. 기본값 'Debug'로 설정합니다.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 설정한 빌드 타입을 기반으로 BUILD_PREFIX_DIR 설정
set(BUILD_PREFIX_DIR "${BUILD_TARGET_OS}/${BUILD_TARGET_ARCH}/${CMAKE_BUILD_TYPE}")

# -----------------------------------------------------------------------------------------------
# 그래픽스 API 옵션 및 플랫폼 분기
# -----------------------------------------------------------------------------------------------
if(USE_OPENGL AND USE_DIRECTX11)
    message(FATAL_ERROR "")
elseif(NOT USE_OPENGL AND NOT USE_DIRECTX11)
    message(FATAL_ERROR "")
endif()

if (APPLE AND USE_DIRECTX11)
    message(FATAL_ERROR "")
endif()

if (USE_OPENGL)
    message(STATUS "Graphics API: OpenGL")
    set(BUILD_RENDER_BACKEND_STR "OPENGL3")
elseif(USE_DIRECTX11)
    message(STATUS "Graphics API: DirectX11")
    set(BUILD_RENDER_BACKEND_STR "DX11")
endif()

# -----------------------------------------------------------------------------------------------
# 소스/헤더 자동 수집
# -----------------------------------------------------------------------------------------------
file(GLOB_RECURSE PROJECT_SOURCE_FILES
    "${ROOT_SRC_DIR}/*.c"
    "${ROOT_SRC_DIR}/*.cc"
    "${ROOT_SRC_DIR}/*.cxx"
    "${ROOT_SRC_DIR}/*.cpp"
)
file(GLOB_RECURSE PROJECT_HEADER_FILES
    "${ROOT_SRC_DIR}/*.h"
    "${ROOT_SRC_DIR}/*.hpp"
    "${ROOT_SRC_DIR}/*.inl"
)

# ThirdParty / PCH 제외
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)ThirdParty(/|\\\\)")
list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)ThirdParty(/|\\\\)")

list(APPEND PROJECT_SOURCE_FILES
    "${THIRD_PARTY_DIR}/tinyfiledialogs/src/tinyfiledialogs.cpp"
)

# -----------------------------------------------------------------------------------------------
# 임시로 renderer 디렉토리 제외
# Renderer/ 전체 제외
# if (WIN32 AND USE_DIRECTX11)
    # Windows + DirectX11 조합에서는 Renderer/ 제외하지 않음
# else()
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)Renderer(/|\\\\)")
list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)Renderer(/|\\\\)")
# list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)SubWindow(/|\\\\)")
# list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)SubWindow(/|\\\\)")
# list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)Runtime/Windows(/|\\\\)")
# list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)Runtime/Windows(/|\\\\)")
# endif()

# 대신 Renderer/Renderer.cpp만 다시 추가
if(EXISTS "${ROOT_SRC_DIR}/Engine/Source/Runtime/Renderer/Renderer.cpp")
    list(APPEND PROJECT_SOURCE_FILES "${ROOT_SRC_DIR}/Engine/Source/Runtime/Renderer/Renderer.cpp")
endif() 


# 백엔드별 소스 필터링(필요 시 경로 패턴 조정)
if (NOT USE_OPENGL)
    list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)(OpenGLDrv||OpenGL|GL)(/|\\\\)")
    list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)(OpenGLDrv||OpenGL|GL)(/|\\\\)")
endif()
if (NOT USE_DIRECTX11)
    list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)(DX11|DirectX11)(/|\\\\)")
    list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)(DX11|DirectX11)(/|\\\\)")
endif()

if (NOT WIN32)
    list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)Windows(/|\\\\)")
    list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)Windows(/|\\\\)")
endif()
if (NOT APPLE)
    list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "(/|\\\\)MacOS(/|\\\\)")
    list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "(/|\\\\)MacOS(/|\\\\)")
endif()

# -----------------------------------------------------------------------------------------------
# Graphics API 의존성
# -----------------------------------------------------------------------------------------------
if (USE_OPENGL)
    find_package(OpenGL REQUIRED)
    # 사전 배포 GLFW (Windows: glfw3, macOS: glfw)
    set(_GLFW_INCLUDE_DIR "${THIRD_PARTY_DIR}/opengl/glfw/include")
    if (WIN32)
        set(_GLFW_LIB_NAME glfw3)
        set(_GLFW_DLL_PATH     "${THIRD_PARTY_DIR}/opengl/glfw/bin/${BUILD_PREFIX_DIR}/glfw3.dll")
        set(_GLFW_LIB_DIR      "${THIRD_PARTY_DIR}/opengl/glfw/lib/${BUILD_PREFIX_DIR}")
    elseif(APPLE)
        set(_GLFW_LIB_NAME glfw)
        set(_GLFW_DYLIB_PATH  "${THIRD_PARTY_DIR}/opengl/glfw/bin/${BUILD_PREFIX_DIR}/glfw3.dll")
        set(_GLFW_LIB_DIR     "${THIRD_PARTY_DIR}/opengl/glfw/lib/${BUILD_PREFIX_DIR}")
    endif()
    set(_GLAD_INCLUDE_DIR "${THIRD_PARTY_DIR}/opengl/glad/include")
    set(_GLAD_SOURCE      "${THIRD_PARTY_DIR}/opengl/glad/src/glad.c")
endif()

# -----------------------------------------------------------------------------------------------
# ImGui 설정
# -----------------------------------------------------------------------------------------------
set (IMGUI_ROOT "${THIRD_PARTY_DIR}/imgui")

set(IMGUI_CORE_SOURCES
    "${IMGUI_ROOT}/imgui.cpp"
    "${IMGUI_ROOT}/imgui_draw.cpp"
    "${IMGUI_ROOT}/imgui_tables.cpp"
    "${IMGUI_ROOT}/imgui_widgets.cpp"
    "${IMGUI_ROOT}/imgui_demo.cpp"
    "${IMGUI_ROOT}/imgui_bezier.cpp"
    "${IMGUI_ROOT}/imgui_curve.cpp"
    "${IMGUI_ROOT}/imgui_neo_internal.cpp"
    "${IMGUI_ROOT}/imgui_neo_sequencer.cpp"
)

set(IMGUI_BACKEND_SOURCES "")
set(IMGUI_BACKEND_LIBS "")

if (USE_DIRECTX11)
    if (NOT WIN32)
        message(FATAL_ERROR "DirectX11은 Windows에서만 사용할 수 있습니다.")
    endif()
    list(APPEND IMGUI_BACKEND_LIBS d3d11 dxgi d3dcompiler)
    list(APPEND IMGUI_BACKEND_SOURCES
        "${IMGUI_ROOT}/backends/imgui_impl_win32.cpp"
        "${IMGUI_ROOT}/backends/imgui_impl_dx11.cpp"
    )
elseif(USE_OPENGL)
    list(APPEND IMGUI_BACKEND_LIBS OpenGL::GL)
    list(APPEND PROJECT_SOURCE_FILES "${_GLAD_SOURCE}")
    if (WIN32)
        list(APPEND IMGUI_BACKEND_LIBS ${_GLFW_LIB_NAME} "${CMAKE_DL_LIBS}")
    else()
        list(APPEND IMGUI_BACKEND_LIBS ${_GLFW_LIB_NAME})
    endif()
    list(APPEND IMGUI_BACKEND_SOURCES
        "${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp"
    )
endif()

add_library(imgui STATIC
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)
target_include_directories(imgui PUBLIC
    "${IMGUI_ROOT}"
    "${IMGUI_ROOT}/backends"
)
if (USE_OPENGL)
    target_include_directories(imgui PUBLIC "${_GLAD_INCLUDE_DIR}")
    if (DEFINED _GLFW_INCLUDE_DIR)
        target_include_directories(imgui PUBLIC "${_GLFW_INCLUDE_DIR}")
    endif()
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD GP_GFX_BACKEND_GL3)
endif()
if (USE_DIRECTX11 AND WIN32)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WIN32_DISABLE_GAMEPAD GP_GFX_BACKEND_DX11)
elseif(APPLE)
    target_compile_definitions(imgui PUBLIC GP_GFX_BACKEND_GL3)
endif()
if (MSVC)
    target_compile_options(imgui PRIVATE /W3 /utf-8)
else()
    target_compile_options(imgui PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()
target_link_libraries(imgui PUBLIC ${IMGUI_BACKEND_LIBS})

# -----------------------------------------------------------------------------------------------
# 실행 파일
# -----------------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
)

# -----------------------------------------------------------------------------------------------
# 출력 디렉터리
# -----------------------------------------------------------------------------------------------
set(_OUT_ROOT "${CMAKE_SOURCE_DIR}/Binaries/${BUILD_TARGET_OS}/${BUILD_TARGET_ARCH}")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${_OUT_ROOT}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_OUT_ROOT}/Release"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    )

# -----------------------------------------------------------------------------------------------
# Include 디렉터리
# -----------------------------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Runtime/"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Runtime/Core/"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/Runtime/Engine/"
        "${THIRD_PARTY_DIR}/imgui"
        "${THIRD_PARTY_DIR}/imgui/backends"
        "${THIRD_PARTY_DIR}/fbxsdk/include"
        "${THIRD_PARTY_DIR}/sol2/include"
        "${THIRD_PARTY_DIR}/lua/include"
        "${THIRD_PARTY_DIR}/tinyfiledialogs/include"

)
if (WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${THIRD_PARTY_DIR}/physx/include"
    )
    if (USE_DIRECTX11)
        target_include_directories(${PROJECT_NAME} PUBLIC
            "${THIRD_PARTY_DIR}/directxtk/include"
        )
    elseif(USE_OPENGL)
        target_include_directories(${PROJECT_NAME} PUBLIC
            "${_GLAD_INCLUDE_DIR}"
            "${_GLFW_INCLUDE_DIR}"
        )
    endif()
elseif(APPLE)
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${_GLAD_INCLUDE_DIR}"
        "${_GLFW_INCLUDE_DIR}"
    )
endif()

# -----------------------------------------------------------------------------------------------
# 컴파일 정의
# -----------------------------------------------------------------------------------------------
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        UNICODE
        _UNICODE
        _CONSOLE
        # BUILD_RENDER_BACKEND=\"${BUILD_RENDER_BACKEND_STR}\"
)
if (USE_OPENGL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        GP_GFX_BACKEND_GL3 
        IMGUI_IMPL_OPENGL_LOADER_GLAD
        USE_OPENGL
        )
elseif (USE_DIRECTX11)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        GP_GFX_BACKEND_DX11
        USE_DIRECTX11
        )
endif()
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FBXSDK_SHARED)
endif()

# -----------------------------------------------------------------------------------------------
# 컴파일/링크 옵션
# -----------------------------------------------------------------------------------------------
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /permissive-
        /MP
        /sdl
        /W3
        /utf-8
        /bigobj
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:Release>:/Gy>
        /Zc:preprocessor
    )
    # WinMain을 사용한다면 WINDOWS 서브시스템이 필요
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wno-unused-parameter
        $<$<CONFIG:Release>:-O2>
    )
endif()

# -----------------------------------------------------------------------------------------------
# 링크 디렉터리 / 라이브러리
# -----------------------------------------------------------------------------------------------
target_link_directories(${PROJECT_NAME} PRIVATE
    "${THIRD_PARTY_DIR}/fbxsdk/lib/${BUILD_PREFIX_DIR}"
    "${THIRD_PARTY_DIR}/fmod/lib/${BUILD_PREFIX_DIR}"
    "${THIRD_PARTY_DIR}/freetype/lib/${BUILD_PREFIX_DIR}"
    # "${THIRD_PARTY_DIR}/lua/lib/${BUILD_PREFIX_DIR}"
    "${THIRD_PARTY_DIR}/sol2/lib/${BUILD_PREFIX_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    freetype
    # lua
    $<$<CONFIG:Debug>:fmodL_vc>
    $<$<NOT:$<CONFIG:Debug>>:fmod_vc>
    libfbxsdk   
    libxml2
    zlib
)

add_definitions(-DFBXSDK_SHARED)

if (WIN32)
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${THIRD_PARTY_DIR}/physx/lib/${BUILD_PREFIX_DIR}"
        "${THIRD_PARTY_DIR}/directxtk/lib/${BUILD_PREFIX_DIR}"
    )
    if (USE_OPENGL)
        target_link_directories(${PROJECT_NAME} PRIVATE "${_GLFW_LIB_DIR}")
    endif()

    set(WINDOWS_LIBS
        DirectXTK
        PhysX_64
        PhysXFoundation_64
        PhysXVehicle_static_64
        PhysXCooking_64
        PhysXExtensions_static_64
        PhysXPvdSDK_static_64         # PVD SDK 추가
        # tinyfiledialogs가 Win32 API를 쓰므로 시스템 라이브러리 추가
        comdlg32 ole32 shell32 uuid user32
    )
    if (USE_OPENGL)
        list(APPEND WINDOWS_LIBS ${_GLFW_LIB_NAME})
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${WINDOWS_LIBS})

elseif(APPLE)
    if (USE_OPENGL)
        target_link_directories(${PROJECT_NAME} PRIVATE "${_GLFW_LIB_DIR}")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${_GLFW_LIB_NAME}
            "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
            OpenGL::GL
        )
    endif()
endif()


get_target_property(LINK_DIRS ${PROJECT_NAME} LINK_DIRECTORIES)
message(STATUS "Link directories for ${PROJECT_NAME}:")

foreach(DIR ${LINK_DIRS})
    message(STATUS " - ${DIR}")
endforeach()

# imgui 링크 (공통)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# -----------------------------------------------------------------------------------------------
# DLL/dylib 복사
# -----------------------------------------------------------------------------------------------
macro(copy_bin LIB_NAME)
    set(SRC_DIR "${THIRD_PARTY_DIR}/${LIB_NAME}/bin/${BUILD_PREFIX_DIR}")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OUT_DIR "${_OUT_ROOT}/Debug")
    else()
        set(OUT_DIR "${_OUT_ROOT}/Release")
    endif()

    # 디렉토리 생성
    file(MAKE_DIRECTORY "${OUT_DIR}")

    # SRC_DIR 안의 모든 파일만 선택 (하위 폴더 제외)
    file(GLOB FILES_TO_COPY "${SRC_DIR}/*")
    message(STATUS "FILES_TO_COPY: ${FILES_TO_COPY}")

    foreach(FILE_PATH ${FILES_TO_COPY})
        get_filename_component(FILE_NAME ${FILE_PATH} NAME)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${FILE_PATH}"
                "${OUT_DIR}/${FILE_NAME}"
            COMMENT "Copying ${FILE_NAME} from ${LIB_NAME}"
        )
    endforeach()
endmacro()



# copy_bin("lua")
copy_bin("fmod")
copy_bin("fbxsdk")

if (WIN32)
    copy_bin("physx")

    if (USE_DIRECTX11)
        copy_bin("directxtk")
    endif()
endif()

if (USE_OPENGL)
        copy_bin("opengl/glfw")
endif()


# -----------------------------------------------------------------------------------------------
# 소스 그룹
# -----------------------------------------------------------------------------------------------
set(ALL_FILES
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)
source_group("ImGui\\Core"    FILES ${IMGUI_CORE_SOURCES})
source_group("ImGui\\Backend" FILES ${IMGUI_BACKEND_SOURCES})
source_group(TREE "${ROOT_SRC_DIR}" FILES ${ALL_FILES})

# target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pch.h")
# --------------------------------------------------------------------------------------------------
