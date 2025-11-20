#pragma once

#include "Core/HAL/PlatformMemory.h"
#include "AssetLoadData.h"

/**
 * Load .mtl file
 */
struct FMaterialLoader
{
    static bool LoadMaterial(const FFilePath& InFilePath, TArray<FMaterialLoadData>& OutLoadResult);
};