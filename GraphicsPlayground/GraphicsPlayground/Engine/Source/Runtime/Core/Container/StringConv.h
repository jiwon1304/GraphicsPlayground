#pragma once
#include <string>
#include <locale>
#include <codecvt>

// variable length -> fixed length (16bit)
inline std::wstring StringToWString(const std::string& String)
{
    if (String.empty())
    {
        return std::wstring{};
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(String);
}

// fixed length (16bit) -> variable length
inline std::string WStringToString(const std::wstring& WString)
{
    if (WString.empty())
    {
        return std::string{};
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(WString);
}

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