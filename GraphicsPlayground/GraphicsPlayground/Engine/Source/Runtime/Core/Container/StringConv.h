#pragma once
#include <string>

#ifdef _MSC_VER
#include <Windows.h>

// variable length -> fixed length (16bit)
inline std::wstring StringToWString(const std::string& String)
{
    if (String.empty())
    {
        return std::wstring{};
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &String[0], (int)String.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &String[0], (int)String.size(), &wstr[0], size_needed);
    return wstr;
}

// fixed length (16bit) -> variable length
inline std::string WStringToString(const std::wstring& WString)
{
    if (WString.empty())
    {
        return std::string{};
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &WString[0], (int)WString.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &WString[0], (int)WString.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

#else

#include <clocale>
#include <cstdlib>

// variable length -> fixed length (16bit)

inline std::wstring StringToWString(const std::string& String)
{
    if (String.empty())
    {
        return std::wstring{};
    }
    std::setlocale(LC_ALL, "en_US.UTF-8");
    size_t len = std::mbstowcs(nullptr, String.c_str(), 0);
    if (len == (size_t)-1) {
        return std::wstring{};
    }
    std::wstring wstr(len, 0);
    std::mbstowcs(&wstr[0], String.c_str(), len);
    return wstr;
}

// fixed length (16bit) -> variable length

inline std::string WStringToString(const std::wstring& WString)
{
    if (WString.empty())
    {
        return std::string{};
    }
    std::setlocale(LC_ALL, "en_US.UTF-8");
    size_t len = std::wcstombs(nullptr, WString.c_str(), 0);
    if (len == (size_t)-1) {
        return std::string{};
    }
    std::string str(len, 0);
    std::wcstombs(&str[0], WString.c_str(), len);
    return str;
}

#endif

/**
 * change FString to const char*
 * usage:
 * TO_UTF8_CHAR(MyString, MyVarName);
 * then use MyVarName as const char*
 * note that MyVarName is valid only in the current scope
 */
#define TO_UTF8_CHAR(str, varName) \
    std::string varName##_utf8 = (str).ToUTF8String(); \
    const char* varName = varName##_utf8.c_str()