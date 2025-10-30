#pragma once

#include "RHI/RHIFwd.h"
#include "RHI/RHIResources.h"
#include "RHI/RHIDefinitions.h"

/**
 * For reflection of shader parameter "types"
 */
template<typename TypeParameter>
struct TShaderParameterTypeInfo
{
    /** 
     * int, uint, float
     * or texture, srv, uav, sampler
     */
	static constexpr EUniformBufferBaseType BaseType;

	/**
     * Data type in shader code will be defined as below.
     * Invalid type will cause compilation error.
     */
	static constexpr int32 NumRows;
	static constexpr int32 NumColumns;

    /**
     * Number of elements if this type is an array.
     * If zero, this type is not an array.
     * If greater than zero, this type is an array.
     */
	static constexpr int32 NumElements;

    /**
     * Alignment in shader parameter struct
     */
    static constexpr int32 Alignment;
};

template<>
struct TShaderParameterTypeInfo<uint32>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_UINT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 1;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 4;

    using TInstancedType = uint32;
};

template<>
struct TShaderParameterTypeInfo<int32>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_INT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 1;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 4;

    using TInstancedType = int32;
};

template<>
struct TShaderParameterTypeInfo<float>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 1;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 4;

    using TInstancedType = float;
};

template<>
struct TShaderParameterTypeInfo<FVector2D>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 2;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 8;

    using TInstancedType = FVector2D;
};

template<>
struct TShaderParameterTypeInfo<FVector>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 3;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 16;

    using TInstancedType = FVector;
};

template<>
struct TShaderParameterTypeInfo<FVector4>
{   
    static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 4;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 16;

    using TInstancedType = FVector4;
};

template<>
struct TShaderParameterTypeInfo<FMatrix>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
    static constexpr int32 NumRows = 4;
    static constexpr int32 NumColumns = 4;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 16;

    using TInstancedType = FMatrix;
};

template<>
struct TShaderParameterTypeInfo<FColor>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_UINT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 4;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 4;

    using TInstancedType = FColor;
};

template<>
struct TShaderParameterTypeInfo<FLinearColor>
{
    static constexpr EUniformBufferBaseType BaseType = UBMT_FLOAT32;
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 4;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 16;

    using TInstancedType = FLinearColor;
};

template<typename T, size_t InNumElements>
struct TShaderParameterTypeInfo<T[InNumElements]>
{
    static constexpr EUniformBufferBaseType BaseType = TShaderParameterTypeInfo<T>::BaseType;
    static constexpr int32 NumRows = TShaderParameterTypeInfo<T>::NumRows;
    static constexpr int32 NumColumns = TShaderParameterTypeInfo<T>::NumColumns;
    static constexpr int32 NumElements = static_cast<int32>(InNumElements);
    static constexpr int32 Alignment = TShaderParameterTypeInfo<T>::Alignment;

    using TInstancedType = T;
};

template<typename ShaderResourceType>
struct TShaderResourceParameterTypeInfo
{
    static constexpr int32 NumRows = 1;
    static constexpr int32 NumColumns = 1;
    static constexpr int32 NumElements = 0;
    static constexpr int32 Alignment = 16;

    using TInstancedType = ShaderResourceType;
};

