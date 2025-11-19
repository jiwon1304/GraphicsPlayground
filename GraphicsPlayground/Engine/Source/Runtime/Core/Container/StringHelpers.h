#include "String.h"

namespace FilePathHelpers
{
FWString GetFileNameNoExtension(const FWString& FilePath)
{
    size_t LastSlashIndex = FilePath.find_last_of(L"/\\");
    size_t LastDotIndex = FilePath.find_last_of(L'.');
    size_t StartIndex = (LastSlashIndex == FWString::npos) ? 0 : LastSlashIndex + 1;
    size_t EndIndex = (LastDotIndex == FWString::npos || LastDotIndex < StartIndex) ? FilePath.length() : LastDotIndex;
    return FilePath.substr(StartIndex, EndIndex - StartIndex);
}

FWString GetFileNameWithExtension(const FWString& FilePath)
{
    size_t LastSlashIndex = FilePath.find_last_of(L"/\\");
    size_t StartIndex = (LastSlashIndex == FWString::npos) ? 0 : LastSlashIndex + 1;
    return FilePath.substr(StartIndex);
}

FWString GetDirectoryPath(const FWString& FilePath)
{
    size_t LastSlashIndex = FilePath.find_last_of(L"/\\");
    if (LastSlashIndex == FWString::npos)
    {
        return L"";
    }
    return FilePath.substr(0, LastSlashIndex + 1);
}

FWString GetFileExtension(const FWString& FilePath)
{
    size_t LastDotIndex = FilePath.find_last_of(L'.');
    if (LastDotIndex == FWString::npos)
    {
        return L"";
    }
    return FilePath.substr(LastDotIndex + 1);
}
} 

namespace StringHelpers
{
    FWString ToLower(const FWString& Input)
    {
        FWString Result = Input;
        std::transform(Result.begin(), Result.end(), Result.begin(), [](wchar_t c) { return std::towlower(c); });
        return Result;
    }
}