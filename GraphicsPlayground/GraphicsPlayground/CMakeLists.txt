# --------------------------------------------------------------------------------------------------
# GraphicsPlayground/CMakeLists.txt (플랫폼/그래픽스 API 선택 완전판)
# - Windows: DirectX11 또는 OpenGL (사용자 선택, 둘 중 하나만)
# - macOS  : OpenGL만 선택 가능 (DirectX11 불가)
# --------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)

project(GraphicsPlayground LANGUAGES C CXX)

option(USE_OPENGL "Use OpenGL backend" ON)
option(USE_DIRECTX11 "Use DirectX 11 backend" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME GraphicsPlayground)
set(ROOT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# -----------------------------------------------------------------------------------------------
# 그래픽스 API 옵션 및 플랫폼 분기
# -----------------------------------------------------------------------------------------------
if (WIN32)
    option(USE_OPENGL    "Use OpenGL backend on Windows" OFF)
    option(USE_DIRECTX11 "Use DirectX 11 backend on Windows" ON)
    # 상호 배타적 선택
    if(USE_OPENGL AND USE_DIRECTX11)
        message(FATAL_ERROR "Windows에서는 OpenGL과 DirectX11을 동시에 활성화할 수 없습니다. 하나만 ON으로 하세요.")
    elseif(NOT USE_OPENGL AND NOT USE_DIRECTX11)
        message(FATAL_ERROR "Windows에서는 OpenGL 또는 DirectX11 중 하나를 반드시 선택해야 합니다.")
    endif()
elseif(APPLE)
    option(USE_OPENGL "Use OpenGL backend on macOS" ON)
    set(USE_DIRECTX11 OFF CACHE BOOL "DirectX11은 macOS에서 지원되지 않습니다." FORCE)
    if(USE_DIRECTX11)
        message(FATAL_ERROR "macOS에서는 DirectX11을 사용할 수 없습니다. USE_DIRECTX11=OFF로 설정하세요.")
    endif()
else()
    message(FATAL_ERROR "지원하지 않는 플랫폼입니다.")
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

# ImGui 경로 및 코어/백엔드 구분
set(IMGUI_ROOT "${ROOT_SRC_DIR}/Engine/Source/ThirdParty/imgui")
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "imgui_impl_.*\\.(cpp|mm)$")
foreach(_core IN ITEMS
        imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp
        imgui_bezier.cpp imgui_curve.cpp imgui_neo_internal.cpp imgui_neo_sequencer.cpp)
    list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX "${_core}$")
endforeach()

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

# -----------------------------------------------------------------------------------------------
# ImGui 백엔드 및 그래픽스 API 분기
# -----------------------------------------------------------------------------------------------
if (WIN32)
    if(USE_DIRECTX11)
        list(APPEND IMGUI_BACKEND_SOURCES
            "${IMGUI_ROOT}/imgui_impl_win32.cpp"
            "${IMGUI_ROOT}/imgui_impl_dx11.cpp"
        )
        list(APPEND IMGUI_BACKEND_LIBS d3d11 dxgi d3dcompiler)
        set(BUILD_RENDER_BACKEND_STR "DX11")
    elseif(USE_OPENGL)
        find_package(OpenGL REQUIRED)
        list(APPEND IMGUI_BACKEND_SOURCES
            "${IMGUI_ROOT}/imgui_impl_win32.cpp"
            "${IMGUI_ROOT}/imgui_impl_opengl3.cpp"
        )
        list(APPEND IMGUI_BACKEND_LIBS OpenGL::GL)
        set(BUILD_RENDER_BACKEND_STR "OPENGL3")
    else()
        message(FATAL_ERROR "Windows에서는 반드시 OpenGL 또는 DirectX11 중 하나를 선택해야 합니다.")
    endif()
elseif(APPLE)
    # macOS: 오직 OpenGL만 가능
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)
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
    list(APPEND IMGUI_BACKEND_LIBS
        ${_GLFW_LINK}
        OpenGL::GL
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
    )
    set(BUILD_RENDER_BACKEND_STR "OPENGL3")
endif()

# -----------------------------------------------------------------------------------------------
# ImGui 정적 라이브러리
# -----------------------------------------------------------------------------------------------
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
if (WIN32)
    if(USE_DIRECTX11)
        target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WIN32_DISABLE_GAMEPAD GP_GFX_BACKEND_DX11)
    elseif(USE_OPENGL)
        target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WIN32_DISABLE_GAMEPAD GP_GFX_BACKEND_GL3)
    endif()
elseif(APPLE)
    target_compile_definitions(imgui PUBLIC GP_GFX_BACKEND_GL3)
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
    "${THIRD_PARTY_DIR}/OpenGL/glad/include"
    "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/include"
    "${THIRD_PARTY_DIR}/stb/stb_image.h"
    )
    endif()
elseif(APPLE)
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${THIRD_PARTY_DIR}/OpenGL/glad/include"
        "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/include"
        "${THIRD_PARTY_DIR}/stb/stb_image.h"
    )
endif()

# -----------------------------------------------------------------------------------------------
# 컴파일 정의(그래픽스 API/플랫폼/출력 등)
# -----------------------------------------------------------------------------------------------
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        UNICODE
        _UNICODE
        _CONSOLE
        BUILD_RENDER_BACKEND=\"${BUILD_RENDER_BACKEND_STR}\"
)
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
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
        /SUBSYSTEM:WINDOWS
    )
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
        "${THIRD_PARTY_DIR}/physx/bin/$<CONFIG>"
        "${THIRD_PARTY_DIR}/freetype/WIN64/lib"
        )

    if (USE_OPENGL)
        list(APPEND THIRD_PARTY_LIB_DIRS
        "${THIRD_PARTY_DIR}/OpenGL/glad/lib/WIN64"
        "${THIRD_PARTY_DIR}/OpenGL/glfw/WIN64/lib-vc2022"
        )
    endif()

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
        )

    if (USE_OPENGL)
        list(APPEND WINDOWS_LIBS
            # glad
            glfw3
        )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${WINDOWS_LIBS})
elseif(APPLE)
    set(THIRD_PARTY_LIB_DIRS
        "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/lib-universal"
    )
    target_link_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_LIB_DIRS})

    set(MACOS_LIBS
        glad
        glfw3
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MACOS_LIBS})
endif()

# imgui 라이브러리 링크 (공통)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# -----------------------------------------------------------------------------------------------
# DLL/dylib 복사 (플랫폼별)
# -----------------------------------------------------------------------------------------------
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
        ################################################
        # @todo : opengl이랑 imgui freetype관련해서 옮기기
    )
elseif(APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "[PostBuild] (macOS) dylib 복사 예시"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "${THIRD_PARTY_DIR}/OpenGL/glfw/MACOS/lib-universal/libglfw.3.dylib" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libglfw.3.dylib"
    )
endif()

# -----------------------------------------------------------------------------------------------
# 소스 그룹 (IDE 보기 개선)
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

# (선택) 프리컴파일 헤더
# target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pch.h")
# --------------------------------------------------------------------------------------------------