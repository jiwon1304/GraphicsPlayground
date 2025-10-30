#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "HAL/PlatformType.h"

struct FColor;
struct FLinearColor;

template <typename T>
struct TVector;

template <typename T>
struct TVector2;

template <typename T>
struct TVector4;

template <typename T>
struct TQuat;

template <typename T>
struct TRotator;

template <typename T>
struct TMatrix;

struct FPlane;

template <typename T>
struct TIntVector3;

template <typename T>
struct TIntVector2;

template <typename T>
struct TIntVector4;

using FVector = TVector<float>;
using FVector2D = TVector2<float>;
using FVector4 = TVector4<float>;
using FQuat = TQuat<float>;
using FRotator = TRotator<float>;
using FMatrix = TMatrix<float>;
using FIntVector3 = TIntVector3<int32>;
using FIntVector2 = TIntVector2<int32>;
using FIntVector4 = TIntVector4<int32>;