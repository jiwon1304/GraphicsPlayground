#pragma once
#include <fstream>

#include "Container/String.h"

struct Serializer
{
    // FString과 FWString의 문자 타입이 wchar_t인지 확인
    static_assert(std::is_same_v<FString::ElementType, wchar_t>, "FString::ElementType must be wchar_t");
    static_assert(std::is_same_v<FWString::value_type, wchar_t>, "FWString::value_type must be wchar_t");
    
    static void WriteFString(std::ofstream& Stream, const FString& InString)
    {
        uint32 Length = InString.Len();
        Stream.write(reinterpret_cast<const char*>(&Length), sizeof(Length));
        Stream.write(reinterpret_cast<const char*>(GetData(InString)), Length * sizeof(FString::ElementType));
    }

    /* Read FString */
    static void ReadFString(std::ifstream& Stream, FString& InString)
    {
        uint32 Length = 0;
        Stream.read(reinterpret_cast<char*>(&Length), sizeof(Length));
        wchar_t* Buffer = new wchar_t[Length + 1];
        Stream.read(reinterpret_cast<char*>(Buffer), Length * sizeof(wchar_t));
        Buffer[Length] = L'\0';
        InString = Buffer;
        delete[] Buffer;
    }

    /* Write FWString */
    static void WriteFWString(std::ofstream& Stream, const FWString& InString)
    {
        uint32 Length = static_cast<uint32>(InString.length());
        Stream.write(reinterpret_cast<const char*>(&Length), sizeof(Length));
        Stream.write(reinterpret_cast<const char*>(InString.c_str()), Length * sizeof(FWString::value_type));
    }

    /* Read FWString */
    static void ReadFWString(std::ifstream& Stream, FWString& InString)
    {
        uint32 Length = 0;
        Stream.read(reinterpret_cast<char*>(&Length), sizeof(Length));
        wchar_t* Buffer = new wchar_t[Length + 1];
        Stream.read(reinterpret_cast<char*>(Buffer), Length * sizeof(wchar_t));
        Buffer[Length] = L'\0';
        InString = Buffer;
        delete[] Buffer;
    }
};
