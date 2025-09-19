# --------------------------------------------------------------------------------------------------
# GraphicsPlayground/CMakeLists.txt (수정판)
# - Windows: DirectX11 + ImGui Win32 Backend
# - macOS  : OpenGL3 + ImGui GLFW Backend
# - ImGui 코어/백엔드 분리, 플랫폼별 라이브러리 및 DLL(또는 dylib) 처리 분기
# - GLOB 사용 (주의: 새 파일 추가 시 CMake 재생성 필요)
# --------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)

project(GraphicsPlayground LANGUAGES C CXX)

option(USE_OPENGL "Use OpenGL backend" ON)
option(USE_DIRECTX11 "Use DirectX 11 backend" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 루트 소스 디렉터리
set(PROJECT_NAME GraphicsPlayground)
set(ROOT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# --------------------------------------------------------------------------------------------------
# 소스 / 헤더 자동 수집
# --------------------------------------------------------------------------------------------------
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

# ImGui 경로
set(IMGUI_ROOT        "${ROOT_SRC_DIR}/Engine/Source/ThirdParty/imgui")

# ImGui 백엔드 / 코어가 메인 소스에 섞이지 않도록 필터링
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "imgui_impl_.*\\.(cpp|mm)$")

foreach(_core IN ITEMS
        imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp
        imgui_bezier.cpp imgui_curve.cpp imgui_neo_internal.cpp imgui_neo_sequencer.cpp)
    list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "${_core}$")
endforeach()

# --------------------------------------------------------------------------------------------------
# ImGui 코어 + 플랫폼별 백엔드
# --------------------------------------------------------------------------------------------------
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

if (WIN32)
    # Windows → DirectX11 + Win32
    list(APPEND IMGUI_BACKEND_SOURCES
        "${IMGUI_ROOT}/imgui_impl_win32.cpp"
        "${IMGUI_ROOT}/imgui_impl_dx11.cpp"
    )
    list(APPEND IMGUI_BACKEND_LIBS d3d11 dxgi d3dcompiler) # 필요시 dinput8, dxguid 등 추가
    set(GP_BACKEND_STR "DX11")
elseif (APPLE)
    # macOS → GLFW + OpenGL3
    # GLFW / OpenGL 찾기
    find_package(OpenGL REQUIRED)          # OpenGL::GL
    find_package(glfw3 QUIET)              # glfw (Config mode)
    if (NOT glfw3_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GLFW3 REQUIRED glfw3)
        add_library(glfw3 INTERFACE)
        target_include_directories(glfw3 INTERFACE ${GLFW3_INCLUDE_DIRS})
        target_link_libraries(glfw3 INTERFACE ${GLFW3_LIBRARIES})
        set(_GLFW_LINK glfw3)
    else()
        set(_GLFW_LINK glfw)
    endif()

    list(APPEND IMGUI_BACKEND_SOURCES
        "${IMGUI_ROOT}/imgui_impl_glfw.cpp"
        "${IMGUI_ROOT}/imgui_impl_opengl3.cpp"
    )

    # macOS 프레임워크 (GLFW가 내부에서 링크할 수도 있지만 명시적으로 추가)
    list(APPEND IMGUI_BACKEND_LIBS
        ${_GLFW_LINK}
        OpenGL::GL
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
    )
    set(GP_BACKEND_STR "OpenGL3")
else()
    message(WARNING "지원되지 않는 플랫폼: 기본 ImGui 코어만 사용 (백엔드 미포함)")
    set(GP_BACKEND_STR "UNKNOWN")
endif()

# --------------------------------------------------------------------------------------------------
# ImGui 정적 라이브러리
# --------------------------------------------------------------------------------------------------
add_library(imgui STATIC
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

target_include_directories(imgui PUBLIC
    "${IMGUI_ROOT}"
    "${IMGUI_ROOT}/include"
)

if (MSVC)
    target_compile_options(imgui PRIVATE /W3 /utf-8)
else()
    target_compile_options(imgui PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

# 플랫폼별 정의 (엔진 코드에서 어떤 백엔드를 쓰는지 알 수 있도록)
if (WIN32)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WIN32_DISABLE_GAMEPAD GP_GFX_BACKEND_DX11)
elseif(APPLE)
    target_compile_definitions(imgui PUBLIC GP_GFX_BACKEND_GL3)
else()
    target_compile_definitions(imgui PUBLIC GP_GFX_BACKEND_UNKNOWN)
endif()

target_link_libraries(imgui PUBLIC ${IMGUI_BACKEND_LIBS})

# --------------------------------------------------------------------------------------------------
# 실행 파일
# --------------------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
)

# --------------------------------------------------------------------------------------------------
# 출력 디렉터리 (Windows x64 기준 예시) - macOS 는 구조 다를 수 있어 수정 가능
# --------------------------------------------------------------------------------------------------
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

# --------------------------------------------------------------------------------------------------
# Include 디렉터리
# --------------------------------------------------------------------------------------------------
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/ThirdParty")

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
        "${THIRD_PARTY_DIR}/OpenGL/glad/include"
)

# Windows 전용 ThirdParty include (DirectXTK, fmod, FBX, physx 등)
if (WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${THIRD_PARTY_DIR}/DirectXTK/Include"
        "${THIRD_PARTY_DIR}/fmod/inc"
        "${THIRD_PARTY_DIR}/physx/include"
        "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/include"
    )
elseif(APPLE)
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${THIRD_PARTY_DIR}/OpenGL/glad/include"
        "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/include"
        "${THIRD_PARTY_DIR}/stb/stb_image.h"
        
        # @TODO : physx 관련해서 추가해야함 
    )
endif()

# --------------------------------------------------------------------------------------------------
# 컴파일 정의
# --------------------------------------------------------------------------------------------------
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        UNICODE
        _UNICODE
        _CONSOLE
        $<$<BOOL:WIN32>:GP_PLATFORM_WINDOWS>
        $<$<BOOL:APPLE>:GP_PLATFORM_MAC>
        GP_RENDER_BACKEND=\"${GP_BACKEND_STR}\"
)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FBXSDK_SHARED)
endif()

# --------------------------------------------------------------------------------------------------
# 컴파일 / 링크 옵션
# --------------------------------------------------------------------------------------------------
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
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
        /SUBSYSTEM:WINDOWS
    )
else()
    # macOS / 기타 (필요시 세분화)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wno-unused-parameter
        $<$<CONFIG:Release>:-O2>
    )
endif()

# --------------------------------------------------------------------------------------------------
# 링크 디렉터리 / 라이브러리
# --------------------------------------------------------------------------------------------------
if (WIN32)
    set(THIRD_PARTY_LIB_DIRS
        "${THIRD_PARTY_DIR}/DirectXTK/Lib/x64/$<CONFIG>"
        "${THIRD_PARTY_DIR}/Lua/lib/$<CONFIG>"
        "${THIRD_PARTY_DIR}/Lua/bin/$<CONFIG>"
        "${THIRD_PARTY_DIR}/fmod/lib/x64"
        "${THIRD_PARTY_DIR}/FBX/lib/x64/$<CONFIG>"
        "${THIRD_PARTY_DIR}/physx/bin/$<CONFIG>"
        "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/lib-vc2022"
        "${THIRD_PARTY_DIR}/freetype/WIN64/lib"
    )
    target_link_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_LIB_DIRS})

    set(WINDOWS_LIBS
        freetype
        DirectXTK
        lua
        fmod_vc
        fmodL_vc
        alembic
        libfbxsdk
        libxml2
        zlib
        PhysX_64
        PhysXFoundation_64
        PhysXExtensions_static_64
        PhysXVehicle_static_64
        PhysXCooking_64
        $<$<CONFIG:Debug>:PhysXPvdSDK_static_64>
        # opengl용 나중에 빼기
        glfw3
        )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${WINDOWS_LIBS})
elseif(APPLE)
    # macOS / 기타 (필요시 세분화)
    set(THIRD_PARTY_LIB_DIRS
        "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/lib-universal"
        # physx lib 경로 필요 시 추가
    )
    target_link_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_LIB_DIRS})

    set(MACOS_LIBS
        glad
        glfw3
        # physx lib 필요 시 추가
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MACOS_LIBS})
endif()

# 공통적으로 imgui 링크 (가장 마지막에)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# --------------------------------------------------------------------------------------------------
# DLL 복사 (Windows 전용)
# --------------------------------------------------------------------------------------------------
if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "[PostBuild] Copy runtime DLLs (Windows)"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${THIRD_PARTY_DIR}/Lua/bin/$<CONFIG>/lua.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/lua.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${THIRD_PARTY_DIR}/fmod/lib/x64/fmod.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fmod.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${THIRD_PARTY_DIR}/fmod/lib/x64/fmodL.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fmodL.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${THIRD_PARTY_DIR}/FBX/lib/x64/$<CONFIG>/libfbxsdk.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libfbxsdk.dll"
    )
endif()

# (macOS 에서 .dylib 복사 필요 시 예시)
if (APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "[PostBuild] (macOS) dylib 복사 예시"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/lib-universal/libglfw.3.dylib" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libglfw.3.dylib"
    )
endif()

# --------------------------------------------------------------------------------------------------
# 소스 그룹 (IDE 보기 개선)
# --------------------------------------------------------------------------------------------------
set(ALL_FILES
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

source_group("ImGui\\Core"    FILES ${IMGUI_CORE_SOURCES})
source_group("ImGui\\Backend" FILES ${IMGUI_BACKEND_SOURCES})
source_group(TREE "${ROOT_SRC_DIR}" FILES ${ALL_FILES})

# --------------------------------------------------------------------------------------------------
# (선택) 프리컴파일 헤더
# target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pch.h")
# --------------------------------------------------------------------------------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}"
)