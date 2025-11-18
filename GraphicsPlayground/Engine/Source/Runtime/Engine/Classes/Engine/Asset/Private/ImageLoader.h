#pragma once

#include "Classes/Engine/Asset/AssetInfo.h"
#include "AssetLoadResult.h"
#include "RHI/PixelFormat.h"

#if defined(BUILD_PLATFORM_WINDOWS)
#include <d3d11.h>
#endif

struct FImageLoader
{
    static bool LoadImage(const FWString& InFilePath, EPixelFormat PixelFormat, FImageLoadResult& OutLoadResult);

#if defined(BUILD_PLATFORM_WINDOWS)
    ID3D11Device* Device = nullptr;
#endif
};