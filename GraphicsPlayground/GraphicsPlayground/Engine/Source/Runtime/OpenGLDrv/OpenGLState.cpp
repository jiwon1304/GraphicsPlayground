#include "OpenGLState.h"
#include "RHI.h"

EFillMode TranslateFillMode(GLenum Mode)
{
    switch (Mode)
    {
    case GL_FILL:   return EFillMode::Solid;
    case GL_LINE:   return EFillMode::Wireframe;
    default:
        assert(false);
        return EFillMode::Solid;
    }
}

ECullMode TranslateCullMode(GLenum Mode)
{
    switch (Mode)
    {
    case GL_FRONT:  return ECullMode::Front;
    case GL_BACK:   return ECullMode::Back;
    case GL_NONE:   return ECullMode::None;
    default:
        assert(false);
        return ECullMode::None;
    }
}

EComparisonFunc TranslateComparisonFunc(GLenum Func)
{
    switch (Func)
    {
    case GL_NEVER:            return EComparisonFunc::Never;
    case GL_LESS:             return EComparisonFunc::Less;
    case GL_EQUAL:            return EComparisonFunc::Equal;
    case GL_LEQUAL:           return EComparisonFunc::LessEqual;
    case GL_GREATER:          return EComparisonFunc::Greater;
    case GL_NOTEQUAL:         return EComparisonFunc::NotEqual;
    case GL_GEQUAL:           return EComparisonFunc::GreaterEqual;
    case GL_ALWAYS:           return EComparisonFunc::Always;
    default:
        assert(false);
        return EComparisonFunc::Always;
    }
}

EStencilOp TranslateStencilOp(GLenum Op)
{
    switch (Op)
    {
    case GL_KEEP:         return EStencilOp::Keep;
    case GL_ZERO:         return EStencilOp::Zero;
    case GL_REPLACE:      return EStencilOp::Replace;
    case GL_INCR:         return EStencilOp::IncrSat;
    case GL_DECR:         return EStencilOp::DecrSat;
    case GL_INVERT:       return EStencilOp::Invert;
    case GL_INCR_WRAP:    return EStencilOp::Incr;
    case GL_DECR_WRAP:    return EStencilOp::Decr;
    default:
        assert(false);
        return EStencilOp::Keep;
    }
}

EBlendFactor TranslateBlendFactor(GLenum Factor)
{
    switch (Factor)
    {
    case GL_ZERO:                  return EBlendFactor::Zero;
    case GL_ONE:                   return EBlendFactor::One;
    case GL_SRC_COLOR:             return EBlendFactor::SrcColor;
    case GL_ONE_MINUS_SRC_COLOR:   return EBlendFactor::InvSrcColor;
    case GL_SRC_ALPHA:             return EBlendFactor::SrcAlpha;
    case GL_ONE_MINUS_SRC_ALPHA:   return EBlendFactor::InvSrcAlpha;
    case GL_DST_ALPHA:             return EBlendFactor::DestAlpha;
    case GL_ONE_MINUS_DST_ALPHA:   return EBlendFactor::InvDestAlpha;
    case GL_DST_COLOR:             return EBlendFactor::DestColor;
    case GL_ONE_MINUS_DST_COLOR:   return EBlendFactor::InvDestColor;
    case GL_CONSTANT_COLOR:        return EBlendFactor::BlendFactor;
    case GL_ONE_MINUS_CONSTANT_COLOR: return EBlendFactor::InvBlendFactor;
    // Note: OpenGL does not have separate Src1 / InvSrc1 factors
    default:
        assert(false);
        return EBlendFactor::One;
    }
}

EBlendOp TranslateBlendOp(GLenum Op)
{
    switch (Op)
    {
    case GL_FUNC_ADD:              return EBlendOp::Add;
    case GL_FUNC_SUBTRACT:         return EBlendOp::Subtract;
    case GL_FUNC_REVERSE_SUBTRACT: return EBlendOp::RevSubtract;
    case GL_MIN:                   return EBlendOp::Min;
    case GL_MAX:                   return EBlendOp::Max;
    default:
        assert(false);
        return EBlendOp::Add;
    }
}

bool FOpenGLRasterizerState::GetInitializer(FRasterizerStateInitializerRHI &Init)
{
    Init.FillMode = TranslateFillMode(Data.FillMode);
    Init.CullMode = TranslateCullMode(Data.CullMode);

    switch (Data.FrontFace)
    {
    case GL_CW:
        Init.bFrontCounterClockwise = false;
        break;
    case GL_CCW:
        Init.bFrontCounterClockwise = true;
        break;
    default:
        assert(false);
        return false;
    }

    Init.DepthBias = static_cast<int32>(Data.DepthBias);
    Init.DepthBiasClamp = Data.DepthBiasClamp;
    Init.SlopeScaledDepthBias = Data.SlopeScaledDepthBias;
    return true;
}

bool FOpenGLDepthStencilState::GetInitializer(FDepthStencilStateInitializerRHI &Init)
{
    Init.bEnableDepth = Data.bZEnable;
    Init.bDepthWriteMask = Data.bZWriteEnable;
    Init.DepthFunc = TranslateComparisonFunc(Data.ZFunc);

    Init.bEnableStencil = Data.bStencilEnable;
    Init.StencilReadMask = static_cast<uint8>(Data.StencilReadMask);
    Init.StencilWriteMask = static_cast<uint8>(Data.StencilWriteMask);

    // Front face
    Init.FrontFaceStencilFunc = TranslateComparisonFunc(Data.StencilFunc);
    Init.FrontFaceStnecilFailOp = TranslateStencilOp(Data.StencilFail);
    Init.FrontFaceStencilDepthFailOp = TranslateStencilOp(Data.StencilZFail);
    Init.FrontFaceStencilPassOp = TranslateStencilOp(Data.StencilPass);

    // Back face
    Init.BackFaceStencilFunc = TranslateComparisonFunc(Data.CCWStencilFunc);
    Init.BackFaceStencilFailOp = TranslateStencilOp(Data.CCWStencilFail);
    Init.BackFaceStencilDepthFailOp = TranslateStencilOp(Data.CCWStencilZFail);
    Init.BackFaceStencilPassOp = TranslateStencilOp(Data.CCWStencilPass);
    return true;
}

bool FOpenGLBlendState::GetInitializer(FBlendStateInitializerRHI &Init)
{
    Init.bAlphaToCoverage = Data.bAlphaBlendEnable;
    Init.bIndependentBlend = Data.bSeparateAlphaBlendEnable;
    Init.bEnableBlend = Data.bAlphaBlendEnable || Data.bSeparateAlphaBlendEnable;
    Init.SrcBlend = TranslateBlendFactor(Data.ColorSourceBlendFactor);
    Init.DestBlend = TranslateBlendFactor(Data.ColorDestBlendFactor);
    Init.BlendOp = TranslateBlendOp(Data.ColorBlendOperation);
    Init.SrcBlendAlpha = TranslateBlendFactor(Data.AlphaSourceBlendFactor);
    Init.DestBlendAlpha = TranslateBlendFactor(Data.AlphaDestBlendFactor);
    Init.BlendOpAlpha = TranslateBlendOp(Data.AlphaBlendOperation);
    Init.RenderTargetWriteMask = Data.RenderTargetWriteMask;
    return true;
}
