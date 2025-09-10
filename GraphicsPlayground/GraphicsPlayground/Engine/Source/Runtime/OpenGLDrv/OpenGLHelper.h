#pragma once

/** Use in source files!! */
#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "ThirdParty/OpenGL/glfw/MacOS/include/GLFW/glfw3.h"
#include "RHI/RHIDefinitions.h"
#include "RHI/RHIResources.h"

GLenum AsGL(EPrimitiveType InType)
{
    switch (InType)
    {
    case EPrimitiveType::PT_TriangleList:   return GL_TRIANGLES;
    case EPrimitiveType::PT_TriangleStrip:  return GL_TRIANGLE_STRIP;
    case EPrimitiveType::PT_LineList:       return GL_LINES;
    case EPrimitiveType::PT_LineStrip:     return GL_LINE_STRIP;
    case EPrimitiveType::PT_PointList:     return GL_POINTS;
    default: assert(false && "Unknown primitive type"); return GL_NONE;
    }
}

GLenum AsGL(EShaderType InType)
{
    switch (InType)
    {
    case EShaderType::Vertex:    return GL_VERTEX_SHADER;
    case EShaderType::Pixel:     return GL_FRAGMENT_SHADER;
    // case EShaderType::Compute:   return GL_COMPUTE_SHADER;
    case EShaderType::Geometry:  return GL_GEOMETRY_SHADER;
    default: assert(false && "Unknown shader type"); return GL_NONE;
    }
}

GLenum AsGL(EBufferUsage InUsage)
{
    switch (InUsage)
    {
    case EBufferUsage::Default:     return GL_STATIC_DRAW;
    case EBufferUsage::Immutable:   return GL_STATIC_DRAW;
    case EBufferUsage::Dynamic:     return GL_DYNAMIC_DRAW;
    case EBufferUsage::Staging:     return GL_STREAM_DRAW;
    default: assert(false && "Unknown buffer usage"); return GL_NONE;
    }
}

GLenum AsGL(ECPUAccess InAccess)
{
    switch (InAccess)
    {
    case ECPUAccess::Read:     return GL_READ_ONLY;
    case ECPUAccess::Write:    return GL_WRITE_ONLY;
    default: assert(false && "Unknown CPU access"); return GL_NONE;
    }
}

GLenum AsGL(EBindFlag InFlag)
{
    switch (InFlag)
    {
    case EBindFlag::None:      return 0;
    case EBindFlag::Vertex:    return GL_ARRAY_BUFFER;
    case EBindFlag::Index:     return GL_ELEMENT_ARRAY_BUFFER;
    // case EBindFlag::SRV:       return GL_SHADER_STORAGE_BUFFER;
    // case EBindFlag::UAV:       return GL_SHADER_STORAGE_BUFFER;
    default: assert(false && "Unknown bind flag"); return GL_NONE;
    }
}

GLenum AsGL(EVertexElementType InType)
{
    switch (InType)
    {
    case EVertexElementType::Float:      return GL_FLOAT;
    case EVertexElementType::Float2:     return GL_FLOAT_VEC2;
    case EVertexElementType::Float3:     return GL_FLOAT_VEC3;
    case EVertexElementType::Float4:     return GL_FLOAT_VEC4;
    case EVertexElementType::Int:        return GL_INT;
    case EVertexElementType::Int2:      return GL_INT_VEC2;
    case EVertexElementType::Int3:      return GL_INT_VEC3;
    case EVertexElementType::Int4:      return GL_INT_VEC4;
    case EVertexElementType::UInt:       return GL_UNSIGNED_INT;
    case EVertexElementType::UInt2:     return GL_UNSIGNED_INT_VEC2;
    case EVertexElementType::UInt3:     return GL_UNSIGNED_INT_VEC3;
    case EVertexElementType::UInt4:     return GL_UNSIGNED_INT_VEC4;
    // case EVertexElementType::Byte:       return GL_BYTE;
    // case EVertexElementType::Short:      return GL_SHORT;
    // case EVertexElementType::Long:       return GL_INT; // GL_LONG 없음, 대신 GL_INT 사용
    // case EVertexElementType::LongLong:   assert(false); // GL_LONG_LONG 없음

    default: assert(false && "Unknown attribute type"); return GL_NONE;
    }
}

/** Watch out for duplicate semantics */
GLenum AsGL(EAttributeUsage InUsage, uint32 InIndex)
{
    switch (InUsage)
    {
    case EAttributeUsage::Position:
        return 0; // 위치는 0번에 매핑
    case EAttributeUsage::Normal:
        return 1;
    case EAttributeUsage::TexCoord:
        assert(InIndex < 3);
        return 2 + InIndex;
    case EAttributeUsage::Color:
        return 5;
    case EAttributeUsage::Tangent:
        return 6;
    case EAttributeUsage::Bitangent:
        return 7;
    case EAttributeUsage::BoneWeights:
        assert(InIndex < 2);
        return 8 + InIndex;
    case EAttributeUsage::BoneIndices:
        assert(InIndex < 2);
        return 10 + InIndex;
    case EAttributeUsage::Custom:
        return 12 + InIndex;
    default: assert(false && "Unknown attribute usage"); return GL_NONE;
    }
}

GLenum AsGL(EBlendOp InOp)
{
    switch (InOp)
    {
    case EBlendOp::Add:          return GL_FUNC_ADD;
    case EBlendOp::Subtract:     return GL_FUNC_SUBTRACT;
    case EBlendOp::RevSubtract:  return GL_FUNC_REVERSE_SUBTRACT;
    case EBlendOp::Min:          return GL_MIN;
    case EBlendOp::Max:          return GL_MAX;
    default: assert(false && "Unknown blend op"); return GL_NONE;
    }
}

GLenum AsGL(EBlendFactor InFactor)
{
    switch (InFactor)
    {
    case EBlendFactor::Zero:                return GL_ZERO;
    case EBlendFactor::One:                 return GL_ONE;
    case EBlendFactor::SrcColor:            return GL_SRC_COLOR;
    case EBlendFactor::InvSrcColor:         return GL_ONE_MINUS_SRC_COLOR;
    case EBlendFactor::SrcAlpha:            return GL_SRC_ALPHA;
    case EBlendFactor::InvSrcAlpha:         return GL_ONE_MINUS_SRC_ALPHA;
    case EBlendFactor::DestAlpha:           return GL_DST_ALPHA;
    case EBlendFactor::InvDestAlpha:        return GL_ONE_MINUS_DST_ALPHA;
    case EBlendFactor::DestColor:           return GL_DST_COLOR;
    case EBlendFactor::InvDestColor:        return GL_ONE_MINUS_DST_COLOR;
    // case EBlendFactor::SrcAlphaSaturate:    return GL_SRC_ALPHA_SATURATE;
    case EBlendFactor::BlendFactor:         return GL_CONSTANT_COLOR;
    case EBlendFactor::InvBlendFactor:      return GL_ONE_MINUS_CONSTANT_COLOR;
    default: assert(false && "Unknown blend factor"); return GL_NONE;
    }
}

GLenum AsGL(EFillMode InFill)
{
    switch (InFill)
    {
    case EFillMode::Wireframe:   return GL_LINE;
    case EFillMode::Solid:       return GL_FILL;
    default: assert(false && "Unknown fill mode"); return GL_NONE;
    }
}

GLenum AsGL(ECullMode InCull)
{
    switch (InCull)
    {
    case ECullMode::None:    return GL_NONE;
    case ECullMode::Front:   return GL_FRONT;
    case ECullMode::Back:    return GL_BACK;
    default: assert(false && "Unknown cull mode"); return GL_NONE;
    }
}

GLenum AsGL(EComparisonFunc InFunc)
{
    switch (InFunc)
    {
    case EComparisonFunc::Never:        return GL_NEVER;
    case EComparisonFunc::Less:         return GL_LESS;
    case EComparisonFunc::Equal:        return GL_EQUAL;
    case EComparisonFunc::LessEqual:    return GL_LEQUAL;
    case EComparisonFunc::Greater:      return GL_GREATER;
    case EComparisonFunc::NotEqual:     return GL_NOTEQUAL;
    case EComparisonFunc::GreaterEqual: return GL_GEQUAL;
    case EComparisonFunc::Always:       return GL_ALWAYS;
    default: assert(false && "Unknown comparison func"); return GL_NONE;
    }
}

GLenum AsGL(EStencilOp InOp)
{
    switch (InOp)
    {
    case EStencilOp::Keep:        return GL_KEEP;
    case EStencilOp::Zero:        return GL_ZERO;
    case EStencilOp::Replace:     return GL_REPLACE;
    case EStencilOp::IncrSat:     return GL_INCR;
    case EStencilOp::DecrSat:     return GL_DECR;
    case EStencilOp::Invert:      return GL_INVERT;
    case EStencilOp::Incr:        return GL_INCR_WRAP;
    case EStencilOp::Decr:        return GL_DECR_WRAP;
    default: assert(false && "Unknown stencil op"); return GL_NONE;
    }
}

// GLenum AsGL(ETextureFilter InFilter)
// {
//     switch (InFilter)
//     {
//     case ETextureFilter::Point:       return GL_NEAREST;
//     case ETextureFilter::Bilinear:   return GL_LINEAR;
//     case ETextureFilter::Trilinear:  return GL_LINEAR_MIPMAP_LINEAR;
//     default: assert(false && "Unknown texture filter"); return GL_NONE;
//     }
// }

GLenum AsGL(ESamplerAddressMode InAddress)
{
    switch (InAddress)
    {
    case ESamplerAddressMode::Wrap:       return GL_REPEAT;
    case ESamplerAddressMode::Clamp:      return GL_CLAMP_TO_EDGE;
    case ESamplerAddressMode::Mirror:     return GL_MIRRORED_REPEAT;
    case ESamplerAddressMode::Border:     return GL_CLAMP_TO_BORDER;
    default: assert(false && "Unknown texture address mode"); return GL_NONE;
    }
}

// GLenum AsGL(ETextureComparisonFunc InFunc)
// {
//     switch (InFunc)
//     {
//     case ETextureComparisonFunc::Never:        return GL_NEVER;
//     case ETextureComparisonFunc::Less:         return GL_LESS;
//     case ETextureComparisonFunc::Equal:        return GL_EQUAL;
//     case ETextureComparisonFunc::LessEqual:    return GL_LEQUAL;
//     case ETextureComparisonFunc::Greater:      return GL_GREATER;
//     case ETextureComparisonFunc::NotEqual:     return GL_NOTEQUAL;
//     case ETextureComparisonFunc::GreaterEqual: return GL_GEQUAL;
//     case ETextureComparisonFunc::Always:       return GL_ALWAYS;
//     default: assert(false && "Unknown texture comparison func"); return GL_NONE;
//     }
// }

GLenum AsGL(ETextureType InType)
{
    switch (InType)
    {
    case ETextureType::Buffer:          assert(false);
    case ETextureType::Texture1D:       return GL_TEXTURE_1D;
    case ETextureType::Texture1DArray:  return GL_TEXTURE_1D_ARRAY;
    case ETextureType::Texture2D:       return GL_TEXTURE_2D;
    case ETextureType::Texture2DArray:  return GL_TEXTURE_2D_ARRAY;
    case ETextureType::Texture3D:       return GL_TEXTURE_3D;
    case ETextureType::Texture3DArray:  assert(false);
    case ETextureType::TextureCube:     return GL_TEXTURE_CUBE_MAP;
    default: assert(false && "Unknown texture type"); return GL_NONE;
    }
}

uint32 GetNumComponents(EVertexElementType InType)
{
    switch (InType)
    {
    case EVertexElementType::Float:      return 1;
    case EVertexElementType::Float2:     return 2;
    case EVertexElementType::Float3:     return 3;
    case EVertexElementType::Float4:     return 4;
    case EVertexElementType::Int:        return 1;
    case EVertexElementType::Int2:      return 2;
    case EVertexElementType::Int3:      return 3;
    case EVertexElementType::Int4:      return 4;
    case EVertexElementType::UInt:       return 1;
    case EVertexElementType::UInt2:     return 2;
    case EVertexElementType::UInt3:     return 3;
    case EVertexElementType::UInt4:     return 4;
    case EVertexElementType::Byte:       return 1;
    case EVertexElementType::Short:      return 1;
    case EVertexElementType::Long:       return 1;
    case EVertexElementType::LongLong:   return 1;

    default: assert(false && "Unknown attribute type"); return 0;
    }
}