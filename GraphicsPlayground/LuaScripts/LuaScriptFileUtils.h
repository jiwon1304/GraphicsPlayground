#pragma once
#include "Core/HAL/PlatformType.h"
#include <filesystem>

#if defined(BUILD_PLATFORM_WINDOWS)
#include <Windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <Shellapi.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")
#endif

namespace fs = std::filesystem;

namespace LuaScriptFileUtils
{
    // template.lua → Scene_Actor.lua 로 복제
    inline bool MakeScriptPathAndDisplayName(
        const std::wstring& templateName,
        const std::wstring& sceneName,
        const std::wstring& actorName,
        FString& outScriptPath,
        FString& outScriptName
    )
    {
        fs::path luaDir = "LuaScripts";
        fs::path src = luaDir / templateName;
        if (!fs::exists(src)) return false;

        fs::path destName = sceneName + L"_" + actorName + L".lua";
        outScriptName = FString(destName.c_str());
        fs::path dst = luaDir / destName;

        outScriptPath = FString(dst.native());
        return true;

        // LPCWSTR luaDir = L"LuaScripts";

        // wchar_t src[MAX_PATH] = { 0 };
        // PathCombineW(src, luaDir, templateName.c_str());
        // if (!PathFileExistsW(src))
        //     return false;

        // // 대상 파일명: Scene_Actor.lua
        // std::wstring destName = sceneName + L"_" + actorName + L".lua";
        // outScriptName = FString(destName.c_str());

        // wchar_t dst[MAX_PATH] = { 0 };
        // PathCombineW(dst, luaDir, destName.c_str());

        // // 복제 (덮어쓰기 허용)
        // /*if (!CopyFileW(src, dst, FALSE))
        //     return false;*/

        // outScriptPath = FString(dst);
        // return true;
    }

#if defined(BUILD_PLATFORM_WINDOWS)
    // 기본 에디터(ShellExecute)로 Lua 파일 열기
    inline void OpenLuaScriptFile(const wchar_t* InLuaFilePath)
    {
        HINSTANCE hInst = ShellExecute(
            NULL,
            _T("open"),
            InLuaFilePath,
            NULL,
            NULL,
            SW_SHOWNORMAL
        );

        if ((INT_PTR)hInst <= 32) {
            MessageBox(NULL, _T("파일 열기에 실패했습니다."), _T("Error"), MB_OK | MB_ICONERROR);
        }
    }
#elif defined(BUILD_PLATFORM_MACOS)
    inline void OpenLuaScriptFile(const wchar_t* InLuaFilePath)
    {
        // Convert wchar_t* to UTF-8 std::string for system command
        std::string path;
        try {
            path = std::filesystem::path(InLuaFilePath).string();
        } catch (...) {
            printf("경로 변환에 실패했습니다.\n");
            return;
        }
        std::string Command = "open '" + path + "'";
        int Result = system(Command.c_str());
        if (Result != 0) {
            // 오류 처리
            printf("파일 열기에 실패했습니다.\n");
        }
    }
#endif
}
