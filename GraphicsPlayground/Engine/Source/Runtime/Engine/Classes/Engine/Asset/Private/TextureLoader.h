#pragma once

#include "Classes/Engine/Asset/AssetInfo.h"
#include "AssetLoadData.h"
#include "RHI/PixelFormat.h"

#if defined(BUILD_PLATFORM_WINDOWS)
#include <d3d11.h>
#endif

struct FTextureLoader
{
    static bool LoadImage(const FFilePath& InFilePath, EPixelFormat PixelFormat, FTextureLoadData& OutLoadResult);

#if defined(BUILD_PLATFORM_WINDOWS)
    ID3D11Device* Device = nullptr;
#endif
};