#pragma once

#include "Core/HAL/PlatformMemory.h"
#include "AssetLoadResult.h"

/**
 * Load .mtl file
 */
struct FMaterialLoader
{
    static bool LoadMaterial(const FFilePath& InFilePath, TArray<FMaterialLoadResult>& OutLoadResult);
};