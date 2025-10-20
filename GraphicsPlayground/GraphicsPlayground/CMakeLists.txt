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

# ImGui root (외부에서 -DIMGUI_ROOT=...로 오버라이드 가능)
if (NOT DEFINED IMGUI_ROOT)
    set(IMGUI_ROOT "${THIRD_PARTY_DIR}/imgui")
endif()

# -----------------------------------------------------------------------------------------------
# 그래픽스 API 옵션 및 플랫폼 분기
# -----------------------------------------------------------------------------------------------
if(USE_OPENGL AND USE_DIRECTX11)
    message(FATAL_ERROR "OpenGL과 DirectX11은 동시에 사용할 수 없습니다. 둘 중 하나만 선택하세요.")
elseif(NOT USE_OPENGL AND NOT USE_DIRECTX11)
    message(FATAL_ERROR "OpenGL 또는 DirectX11 중 하나를 반드시 선택해야 합니다.")
endif()

if (APPLE AND USE_DIRECTX11)
    message(FATAL_ERROR "macOS에서는 DirectX11을 사용할 수 없습니다. USE_DIRECTX11=OFF로 설정하세요.")
endif()

if (USE_OPENGL)
    message(STATUS "그래픽스 API: OpenGL 선택됨")
    set(BUILD_RENDER_BACKEND_STR "OPENGL3")
elseif(USE_DIRECTX11)
    message(STATUS "그래픽스 API: DirectX11 선택됨")
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
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "pch\\.cpp$")
list(FILTER PROJECT_HEADER_FILES EXCLUDE REGEX "pch\\.h$")

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
    if (WIN32)
        set(_GLFW_LIB_NAME glfw3)
        set(_GLFW_INCLUDE_DIR "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/include")
        set(_GLFW_DLL_PATH     "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/lib-vc2022/glfw3.dll")
        set(_GLFW_LIB_DIR      "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/lib-vc2022")
    elseif(APPLE)
        set(_GLFW_LIB_NAME glfw)
        set(_GLFW_INCLUDE_DIR "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/include")
        set(_GLFW_DYLIB_PATH  "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/lib-universal/libglfw.3.dylib")
        set(_GLFW_LIB_DIR     "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/lib-universal")
    endif()
    set(_GLAD_INCLUDE_DIR "${THIRD_PARTY_DIR}/OpenGL/glad/include")
    set(_GLAD_SOURCE      "${THIRD_PARTY_DIR}/OpenGL/glad/src/glad.c")
endif()

# -----------------------------------------------------------------------------------------------
# tinyfiledialogs (단일 C 파일) 추가
# -----------------------------------------------------------------------------------------------
set(TINYFD_HEADER "${THIRD_PARTY_DIR}/tinyfiledialogs/include/tinyfiledialogs.h")
set(TINYFD_SOURCE "${THIRD_PARTY_DIR}/tinyfiledialogs/include/tinyfiledialogs.cpp")
if (EXISTS "${TINYFD_SOURCE}")
    list(APPEND PROJECT_SOURCE_FILES "${TINYFD_SOURCE}")
else()
    message(WARNING "tinyfiledialogs.c를 찾을 수 없습니다: ${TINYFD_SOURCE}
tinyfd_* 링커 에러가 나면 tinyfiledialogs.c를 해당 경로에 추가하세요.
공식 배포에서 tinyfiledialogs.c/.h를 함께 가져오면 됩니다: https://sourceforge.net/projects/tinyfiledialogs/")
endif()

# -----------------------------------------------------------------------------------------------
# ImGui 설정
# -----------------------------------------------------------------------------------------------
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
if (WIN32)
    set(_OUT_ROOT "${CMAKE_SOURCE_DIR}/Binaries/x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${_OUT_ROOT}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_OUT_ROOT}/Release"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    )
elseif(APPLE)
    set(_OUT_ROOT "${CMAKE_SOURCE_DIR}/Binaries/Apple")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${_OUT_ROOT}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_OUT_ROOT}/Release"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    )
endif()

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
        "${THIRD_PARTY_DIR}/freetype/include"
        "${THIRD_PARTY_DIR}/Lua/include"
        "${THIRD_PARTY_DIR}/sol2/include"
        "${THIRD_PARTY_DIR}/magic_enum/include"
        "${THIRD_PARTY_DIR}/Json/include"
        "${THIRD_PARTY_DIR}/tinyfiledialogs/include"
        "${THIRD_PARTY_DIR}/FBX/include"
        "${THIRD_PARTY_DIR}/stbimage"
)
if (WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${THIRD_PARTY_DIR}/fmod/inc"
        "${THIRD_PARTY_DIR}/physx/include"
    )
    if (USE_DIRECTX11)
        target_include_directories(${PROJECT_NAME} PUBLIC
            "${THIRD_PARTY_DIR}/DirectXTK/Include"
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
if (WIN32)
    set(THIRD_PARTY_LIB_DIRS
        "${THIRD_PARTY_DIR}/DirectXTK/Lib/x64/$<CONFIG>"
        "${THIRD_PARTY_DIR}/Lua/lib/$<CONFIG>"
        "${THIRD_PARTY_DIR}/Lua/bin/$<CONFIG>"
        "${THIRD_PARTY_DIR}/fmod/lib/x64"
        "${THIRD_PARTY_DIR}/FBX/lib/x64/$<CONFIG>"
        "$<IF:$<CONFIG:Debug>,${THIRD_PARTY_DIR}/physx/bin/debug,${THIRD_PARTY_DIR}/physx/bin/release>"
        "${THIRD_PARTY_DIR}/freetype/WIN64/lib"
    )
    if (USE_OPENGL)
        list(APPEND THIRD_PARTY_LIB_DIRS "${_GLFW_LIB_DIR}")
    endif()
    target_link_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_LIB_DIRS})

    set(WINDOWS_LIBS
        freetype
        DirectXTK
        lua
        $<$<CONFIG:Debug>:fmodL_vc>
        $<$<NOT:$<CONFIG:Debug>>:fmod_vc>
        libfbxsdk
        libxml2
        zlib
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

# imgui 링크 (공통)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# -----------------------------------------------------------------------------------------------
# DLL/dylib 복사
# -----------------------------------------------------------------------------------------------
if (WIN32)
    # 헬퍼 스크립트 생성: 단일 파일 복사(존재하면), 디렉터리 내 *.dll 일괄 복사
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake" "
if(NOT DEFINED SRC OR NOT DEFINED DST_DIR)
  message(FATAL_ERROR \"CopyIfExists: SRC, DST_DIR가 필요합니다.\")
endif()
if(EXISTS \"${SRC}\")
  file(COPY \"${SRC}\" DESTINATION \"${DST_DIR}\")
  message(STATUS \"[PostBuild] Copied: ${SRC} -> ${DST_DIR}\")
else()
  message(STATUS \"[PostBuild] Skip (not found): ${SRC}\")
endif()
")

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CopyDirDllsIfExists.cmake" "
if(NOT DEFINED SRC_DIR OR NOT DEFINED DST_DIR)
  message(FATAL_ERROR \"CopyDirDllsIfExists: SRC_DIR, DST_DIR가 필요합니다.\")
endif()
if(EXISTS \"${SRC_DIR}\")
  file(GLOB DLLS \"${SRC_DIR}/*.dll\")
  foreach(f IN LISTS DLLS)
    file(COPY \"${f}\" DESTINATION \"${DST_DIR}\")
    message(STATUS \"[PostBuild] Copied: ${f} -> ${DST_DIR}\")
  endforeach()
else()
  message(STATUS \"[PostBuild] Skip dir (not found): ${SRC_DIR}\")
endif()
")

    # 구성별 PhysX bin 폴더 선택
    set(_PHYSX_BIN_DIR "$<IF:$<CONFIG:Debug>,${THIRD_PARTY_DIR}/physx/bin/debug,${THIRD_PARTY_DIR}/physx/bin/release>")

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "[PostBuild] Copy runtime DLLs (Windows)"

        # Lua
        COMMAND ${CMAKE_COMMAND} -DSRC="${THIRD_PARTY_DIR}/Lua/bin/$<CONFIG>/lua.dll" -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake"

        # fmod
        COMMAND ${CMAKE_COMMAND} -DSRC="${THIRD_PARTY_DIR}/fmod/lib/x64/fmod.dll"  -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake"
        COMMAND ${CMAKE_COMMAND} -DSRC="${THIRD_PARTY_DIR}/fmod/lib/x64/fmodL.dll" -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake"

        # FBX
        COMMAND ${CMAKE_COMMAND} -DSRC="${THIRD_PARTY_DIR}/FBX/lib/x64/$<CONFIG>/libfbxsdk.dll" -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake"

        # PhysX: bin/<cfg>의 모든 dll 일괄 복사(없으면 통과)
        COMMAND ${CMAKE_COMMAND} -DSRC_DIR="${_PHYSX_BIN_DIR}" -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyDirDllsIfExists.cmake"

        # FreeType
        COMMAND ${CMAKE_COMMAND} -DSRC="${THIRD_PARTY_DIR}/freetype/WIN64/bin/freetype.dll" -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake"
    )

    # OpenGL일 때에만 GLFW DLL 복사(존재 시)
    if (USE_OPENGL)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "[PostBuild] (Windows) Copy GLFW DLL"
            COMMAND ${CMAKE_COMMAND} -DSRC="${_GLFW_DLL_PATH}" -DDST_DIR="$<TARGET_FILE_DIR:${PROJECT_NAME}>" -P "${CMAKE_CURRENT_BINARY_DIR}/CopyIfExists.cmake"
        )
    endif()

elseif(APPLE)
    if (USE_OPENGL)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "[PostBuild] (macOS) Copy GLFW dylib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                "${_GLFW_DYLIB_PATH}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libglfw.3.dylib"
        )
    endif()
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