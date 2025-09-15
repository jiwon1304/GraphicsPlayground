#include "OpenGLState.h"
#include "RHI/RHI.h"
#include "OpenGLDrv.h"

// cannot be referenced outside of this translation unit (= .cpp file)
// we cache sampler states to avoid redundant creations and bindings
static TMap<FSamplerStateInitializerRHI, FOpenGLSamplerState*> GSamplerStateCache;

// Utility functions for translating between engine enums and OpenGL enums
static ESamplerAddressMode TranslateAddressMode(GLenum Mode)
{
    switch (Mode)
    {
    case GL_REPEAT:          return ESamplerAddressMode::Wrap;
    case GL_MIRRORED_REPEAT: return ESamplerAddressMode::Mirror;
    case GL_CLAMP_TO_EDGE:   return ESamplerAddressMode::Clamp;
    case GL_CLAMP_TO_BORDER: return ESamplerAddressMode::Border;
    default:
        assert(false);
        return ESamplerAddressMode::Wrap;
    }
}

static GLenum TranslateAddressMode(ESamplerAddressMode Mode)
{
    switch (Mode)
    {
    case ESamplerAddressMode::Wrap:   return GL_REPEAT;
    case ESamplerAddressMode::Mirror: return GL_MIRRORED_REPEAT;
    case ESamplerAddressMode::Clamp:  return GL_CLAMP_TO_EDGE;
    case ESamplerAddressMode::Border: return GL_CLAMP_TO_BORDER;
    default:
        assert(false);
        return GL_REPEAT;
    }
}

static EFillMode TranslateFillMode(GLenum Mode)
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

static GLenum TranslateFillMode(EFillMode Mode)
{
    switch (Mode)
    {
    case EFillMode::Solid:      return GL_FILL;
    case EFillMode::Wireframe:  return GL_LINE;
    default:
        assert(false);
        return GL_FILL;
    }
}

static ECullMode TranslateCullMode(GLenum Mode)
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

static GLenum TranslateCullMode(ECullMode Mode)
{
    switch (Mode)
    {
    case ECullMode::Front:   return GL_FRONT;
    case ECullMode::Back:    return GL_BACK;
    case ECullMode::None:    return GL_NONE;
    default:
        assert(false);
        return GL_NONE;
    }
}

static EComparisonFunc TranslateComparisonFunc(GLenum Func)
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

static GLenum TranslateComparisonFunc(EComparisonFunc Func)
{
    switch (Func)
    {
    case EComparisonFunc::Never:          return GL_NEVER;
    case EComparisonFunc::Less:           return GL_LESS;
    case EComparisonFunc::Equal:          return GL_EQUAL;
    case EComparisonFunc::LessEqual:      return GL_LEQUAL;
    case EComparisonFunc::Greater:        return GL_GREATER;
    case EComparisonFunc::NotEqual:       return GL_NOTEQUAL;
    case EComparisonFunc::GreaterEqual:   return GL_GEQUAL;
    case EComparisonFunc::Always:         return GL_ALWAYS;
    default:
        assert(false);
        return GL_ALWAYS;
    }
}

static EStencilOp TranslateStencilOp(GLenum Op)
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

static GLenum TranslateStencilOp(EStencilOp Op)
{
    switch (Op)
    {
    case EStencilOp::Keep:      return GL_KEEP;
    case EStencilOp::Zero:      return GL_ZERO;
    case EStencilOp::Replace:   return GL_REPLACE;
    case EStencilOp::IncrSat:   return GL_INCR;
    case EStencilOp::DecrSat:   return GL_DECR;
    case EStencilOp::Invert:    return GL_INVERT;
    case EStencilOp::Incr:      return GL_INCR_WRAP;
    case EStencilOp::Decr:      return GL_DECR_WRAP;
    default:
        assert(false);
        return GL_KEEP;
    }
}

static EBlendFactor TranslateBlendFactor(GLenum Factor)
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

static GLenum TranslateBlendFactor(EBlendFactor Factor)
{
    switch (Factor)
    {
    case EBlendFactor::Zero:            return GL_ZERO;
    case EBlendFactor::One:             return GL_ONE;
    case EBlendFactor::SrcColor:        return GL_SRC_COLOR;
    case EBlendFactor::InvSrcColor:     return GL_ONE_MINUS_SRC_COLOR;
    case EBlendFactor::SrcAlpha:        return GL_SRC_ALPHA;
    case EBlendFactor::InvSrcAlpha:     return GL_ONE_MINUS_SRC_ALPHA;
    case EBlendFactor::DestAlpha:       return GL_DST_ALPHA;
    case EBlendFactor::InvDestAlpha:    return GL_ONE_MINUS_DST_ALPHA;
    case EBlendFactor::DestColor:       return GL_DST_COLOR;
    case EBlendFactor::InvDestColor:    return GL_ONE_MINUS_DST_COLOR;
    case EBlendFactor::BlendFactor:     return GL_CONSTANT_COLOR;
    case EBlendFactor::InvBlendFactor:  return GL_ONE_MINUS_CONSTANT_COLOR;
    // Note: OpenGL does not have separate Src1 / InvSrc1 factors
    default:
        assert(false);
        return GL_ONE;
    }
}

static EBlendOp TranslateBlendOp(GLenum Op)
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

static GLenum TranslateBlendOp(EBlendOp Op)
{
    switch (Op)
    {
    case EBlendOp::Add:            return GL_FUNC_ADD;
    case EBlendOp::Subtract:       return GL_FUNC_SUBTRACT;
    case EBlendOp::RevSubtract:    return GL_FUNC_REVERSE_SUBTRACT;
    case EBlendOp::Min:            return GL_MIN;
    case EBlendOp::Max:            return GL_MAX;
    default:
        assert(false);
        return GL_FUNC_ADD;
    }
}

// -----------------------------------------------------------------------------
// Sampler States
// -----------------------------------------------------------------------------
FSamplerStateRHIRef FOpenGLDynamicRHI::RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer)
{
    if (GSamplerStateCache.Contains(Initializer))
    {
        return GSamplerStateCache[Initializer];
    }

    FOpenGLSamplerState* SamplerState = static_cast<FOpenGLSamplerState*>(FPlatformMemory::Malloc<EAT_RHI>(sizeof(FOpenGLSamplerState)));
    new (SamplerState) FOpenGLSamplerState();
    
    SamplerState->Data.WrapS = TranslateAddressMode(Initializer.AddressU);
    SamplerState->Data.WrapT = TranslateAddressMode(Initializer.AddressV);
    SamplerState->Data.WrapR = TranslateAddressMode(Initializer.AddressW);

    switch (Initializer.Filter)
    {
    case ESamplerFilter::Point:
        SamplerState->Data.MagFilter = GL_NEAREST;
        SamplerState->Data.MinFilter = GL_NEAREST_MIPMAP_NEAREST;
        break;
    case ESamplerFilter::Bilinear:
        SamplerState->Data.MagFilter = GL_LINEAR;
        SamplerState->Data.MinFilter = GL_LINEAR;
        break;
    case ESamplerFilter::Trilinear:
        SamplerState->Data.MagFilter = GL_LINEAR_MIPMAP_LINEAR;
        SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_LINEAR;
        break;
    case ESamplerFilter::Anisotropic:
        assert(false && "Anisotropic filter mode not available in OpenGL3.3");
        SamplerState->Data.MagFilter = GL_LINEAR_MIPMAP_LINEAR;
        SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_LINEAR;
        break;
    default:
        assert(false);
    }

    SamplerState->Data.CompareMode = Initializer.bUseComparison ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE;
    SamplerState->Data.CompareFunc = Initializer.bUseComparison ? GL_LEQUAL : GL_NONE;

    // Sampler state is an object in OpenGL, so we treat it unlike other states.
    FRHICommandListImmediate::Get().EnqueueLambda([SamplerState]()
    {
        FOpenGL::GenSamplers(1, &SamplerState->Resource);

        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_WRAP_S, SamplerState->Data.WrapS);
        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_WRAP_T, SamplerState->Data.WrapT);
        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_WRAP_R, SamplerState->Data.WrapR);

        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_MIN_FILTER, SamplerState->Data.MinFilter);
        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_MAG_FILTER, SamplerState->Data.MagFilter);

        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_COMPARE_MODE, SamplerState->Data.CompareMode);
        FOpenGL::SetSamplerParameter(SamplerState->Resource, GL_TEXTURE_COMPARE_FUNC, SamplerState->Data.CompareFunc);
    });

    FSamplerStateRHIRef SamplerStateRef = SamplerState;
    GSamplerStateCache.Add(Initializer, SamplerState);

    return SamplerStateRef;
}

// -----------------------------------------------------------------------------
// Rasterizer States
// -----------------------------------------------------------------------------
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

FRasterizerStateRHIRef FOpenGLDynamicRHI::RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer)
{
    FOpenGLRasterizerState* RasterizerState = static_cast<FOpenGLRasterizerState*>(FPlatformMemory::Malloc<EAT_RHI>(sizeof(FOpenGLRasterizerState)));
    new (RasterizerState) FOpenGLRasterizerState();

    RasterizerState->Data.FillMode = TranslateFillMode(Initializer.FillMode);
    RasterizerState->Data.CullMode = TranslateCullMode(Initializer.CullMode);
    RasterizerState->Data.FrontFace = Initializer.bFrontCounterClockwise ? GL_CCW : GL_CW;
    RasterizerState->Data.DepthBias = static_cast<GLfloat>(Initializer.DepthBias);
    RasterizerState->Data.DepthBiasClamp = Initializer.DepthBiasClamp;
    RasterizerState->Data.SlopeScaledDepthBias = Initializer.SlopeScaledDepthBias;

    return RasterizerState;
}

// -----------------------------------------------------------------------------
// Depth Stencil States
// -----------------------------------------------------------------------------

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

FDepthStencilStateRHIRef FOpenGLDynamicRHI::RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer)
{
    FOpenGLDepthStencilState* DepthStencilState = static_cast<FOpenGLDepthStencilState*>(FPlatformMemory::Malloc<EAT_RHI>(sizeof(FOpenGLDepthStencilState)));
    new (DepthStencilState) FOpenGLDepthStencilState();

    DepthStencilState->Data.bZEnable = Initializer.bEnableDepth;
    DepthStencilState->Data.bZWriteEnable = Initializer.bDepthWriteMask;
    DepthStencilState->Data.ZFunc = TranslateComparisonFunc(Initializer.DepthFunc);

    DepthStencilState->Data.bStencilEnable = Initializer.bEnableStencil;
    DepthStencilState->Data.StencilReadMask = Initializer.StencilReadMask;
    DepthStencilState->Data.StencilWriteMask = Initializer.StencilWriteMask;

    // Front face
    DepthStencilState->Data.StencilFunc = TranslateComparisonFunc(Initializer.FrontFaceStencilFunc);
    DepthStencilState->Data.StencilFail = TranslateStencilOp(Initializer.FrontFaceStnecilFailOp);
    DepthStencilState->Data.StencilZFail = TranslateStencilOp(Initializer.FrontFaceStencilDepthFailOp);
    DepthStencilState->Data.StencilPass = TranslateStencilOp(Initializer.FrontFaceStencilPassOp);

    // Back face
    DepthStencilState->Data.CCWStencilFunc = TranslateComparisonFunc(Initializer.BackFaceStencilFunc);
    DepthStencilState->Data.CCWStencilFail = TranslateStencilOp(Initializer.BackFaceStencilFailOp);
    DepthStencilState->Data.CCWStencilZFail = TranslateStencilOp(Initializer.BackFaceStencilDepthFailOp);
    DepthStencilState->Data.CCWStencilPass = TranslateStencilOp(Initializer.BackFaceStencilPassOp);

    return DepthStencilState;
}

// -----------------------------------------------------------------------------
// Blend States
// -----------------------------------------------------------------------------

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

FBlendStateRHIRef FOpenGLDynamicRHI::RHICreateBlendState(const FBlendStateInitializerRHI& Initializer)
{
    FOpenGLBlendState* BlendState = static_cast<FOpenGLBlendState*>(FPlatformMemory::Malloc<EAT_RHI>(sizeof(FOpenGLBlendState)));
    new (BlendState) FOpenGLBlendState();

    BlendState->Data.bAlphaBlendEnable = Initializer.bEnableBlend;
    BlendState->Data.ColorBlendOperation = TranslateBlendOp(Initializer.BlendOp);
    BlendState->Data.ColorSourceBlendFactor = TranslateBlendFactor(Initializer.SrcBlend);
    BlendState->Data.ColorDestBlendFactor = TranslateBlendFactor(Initializer.DestBlend);
    BlendState->Data.bSeparateAlphaBlendEnable = Initializer.bIndependentBlend;
    BlendState->Data.AlphaBlendOperation = TranslateBlendOp(Initializer.BlendOpAlpha);
    BlendState->Data.AlphaSourceBlendFactor = TranslateBlendFactor(Initializer.SrcBlendAlpha);
    BlendState->Data.AlphaDestBlendFactor = TranslateBlendFactor(Initializer.DestBlendAlpha);
    BlendState->Data.RenderTargetWriteMask = Initializer.RenderTargetWriteMask;

    return BlendState;
}
