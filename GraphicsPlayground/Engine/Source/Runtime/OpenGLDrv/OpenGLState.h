#pragma once
#include "OpenGLThirdParty.h"
#include "OpenGLResources.h"
#include "Core/Math/Rect.h"
#include "Core/Container/Map.h"

// struct FOpenGLRHIState
// {
//     GLuint VertexArrayId;
//     GLuint FramebufferId;
//     GLuint RenderbufferId;
//     GLuint ProgramId;
//     GLuint TextureId;
//     GLuint SamplerId;
// };

struct FOpenGLSamplerStateData
{
    GLint WrapS;
    GLint WrapT;
    GLint WrapR;
    GLint MinFilter;
    GLint MagFilter;
    GLint CompareMode;
    GLint CompareFunc;

    FOpenGLSamplerStateData()
        : WrapS(GL_REPEAT)
        , WrapT(GL_REPEAT)
        , WrapR(GL_REPEAT)
        , MinFilter(GL_LINEAR_MIPMAP_LINEAR)
        , MagFilter(GL_LINEAR)
        , CompareMode(GL_NONE)
        , CompareFunc(GL_LEQUAL)
    {}
};

class FOpenGLSamplerState : public FRHISamplerState
{
public:
    GLuint Resource;
    FOpenGLSamplerStateData Data;

    ~FOpenGLSamplerState()
    {
        FOpenGL::DeleteSamplers(1, &Resource);
    }
};

struct FOpenGLRasterizerStateData
{
    GLenum FillMode;
    GLenum CullMode;
    GLenum FrontFace;
    GLfloat DepthBias = 0.f;
    GLfloat DepthBiasClamp = 0.f;
    GLfloat SlopeScaledDepthBias = 0.f;
};

class FOpenGLRasterizerState : public FRHIRasterizerState
{
public:
    FOpenGLRasterizerStateData Data;

	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) override final;
};

struct FOpenGLDepthStencilStateData
{
	bool bZEnable;
	bool bZWriteEnable;
	GLenum ZFunc;
	
	bool bStencilEnable;
	bool bTwoSidedStencilMode;
	GLenum StencilFunc;
	GLenum StencilFail;
	GLenum StencilZFail;
	GLenum StencilPass;
	GLenum CCWStencilFunc;
	GLenum CCWStencilFail;
	GLenum CCWStencilZFail;
	GLenum CCWStencilPass;
	uint32 StencilReadMask;
	uint32 StencilWriteMask;
};

class FOpenGLDepthStencilState : public FRHIDepthStencilState
{
public:
    FOpenGLDepthStencilStateData Data;

	virtual bool GetInitializer(struct FDepthStencilStateInitializerRHI& Init);
};

struct FOpenGLBlendStateData
{
    bool bAlphaBlendEnable;
    GLenum ColorBlendOperation;
    GLenum ColorSourceBlendFactor;
    GLenum ColorDestBlendFactor;
    bool bSeparateAlphaBlendEnable;
    GLenum AlphaBlendOperation;
    GLenum AlphaSourceBlendFactor;
    GLenum AlphaDestBlendFactor;
    uint8 RenderTargetWriteMask; // RGBA
};

class FOpenGLBlendState : public FRHIBlendState
{
public:
    FOpenGLBlendStateData Data;

    virtual bool GetInitializer(class FBlendStateInitializerRHI& Init) override final;
};

struct FTextureStage
{
    class FOpenGLTexture* Texture = nullptr;
    class FOpenGLShaderResourceView* SRV = nullptr;
};

// No correspondence in OpenGL3.3
// struct FUAVStage
// {
// }

struct FOpenGLCachedAttr
{
    GLuint Size = 0;
    GLenum Type;
    GLuint StreamOffset;
    GLuint StreamIndex;
    GLboolean bNormalized;
};

/**
 * currently bound resources, states
 */
struct FOpenGLCommonState
{
    // use same index
    TArray<FTextureStage> Textures;
    TArray<FOpenGLSamplerState*> SamplerStates;
};

struct FOpenGLContextState final : public FOpenGLCommonState
{
	FOpenGLRasterizerStateData		RasterizerState;
	FOpenGLDepthStencilStateData	DepthStencilState;
	uint32							StencilRef;
	FOpenGLBlendStateData			BlendState;
	GLuint							Framebuffer;
	FIntRect						Viewport;
	float							DepthMinZ;
	float							DepthMaxZ;
	uint32							RenderTargetWidth;
	uint32							RenderTargetHeight;
	bool							bAlphaToCoverageEnabled;

	// Pending framebuffer setup
	FOpenGLTexture*					RenderTargets[MaxSimultaneousRenderTargets];
	FOpenGLTexture*					DepthStencil;
	ERenderTargetStoreAction		StencilStoreAction;
	uint32							DepthTargetWidth;
	uint32							DepthTargetHeight;
	bool							bFramebufferSetupInvalid;

	TRefCountPtr<FOpenGLBoundShaderState>	BoundShaderState;
	TRefCountPtr<FOpenGLComputeShader>		CurrentComputeShader;
};