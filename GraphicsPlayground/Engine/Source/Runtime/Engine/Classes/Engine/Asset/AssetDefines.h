#pragma once
#include <filesystem>
#include "Core/HAL/PlatformType.h"

typedef std::filesystem::path FFilePath;
typedef uint16 IndexType;
constexpr IndexType INDEXTYPE_MAX = std::numeric_limits<IndexType>::max();