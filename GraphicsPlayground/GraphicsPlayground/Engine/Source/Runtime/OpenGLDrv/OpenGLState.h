#pragma once
#include "ThirdParty/OpenGL/glad/include/glad/glad.h"
#include "OpenGLResources.h"
#include "Core/Math/Rect.h"

struct FOpenGLRHIState
{
    GLuint VertexArrayId;
    GLuint FramebufferId;
    GLuint RenderbufferId;
    GLuint ProgramId;
    GLuint TextureId;
    GLuint SamplerId;
};

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

struct FOpenGLRHIState final : public FOpenGLCommonState
{
	FOpenGLRasterizerStateData		RasterizerState;
	FOpenGLDepthStencilStateData	DepthStencilState;
	uint32							StencilRef;
	FOpenGLBlendStateData			BlendState;
	GLuint							Framebuffer;
	bool							bScissorEnabled;
	FIntRect						Scissor;
	FIntRect						Viewport;
	float							DepthMinZ;
	float							DepthMaxZ;
	GLuint							ZeroFilledDummyUniformBuffer;
	uint32							RenderTargetWidth;
	uint32							RenderTargetHeight;
	bool							bAlphaToCoverageEnabled;

	// Pending framebuffer setup
	int32							NumRenderingSamples;// Only used with GL_EXT_multisampled_render_to_texture
	int32							FirstNonzeroRenderTarget;
	FOpenGLTexture*					RenderTargets[MaxSimultaneousRenderTargets];
	uint32							RenderTargetMipmapLevels[MaxSimultaneousRenderTargets];
	uint32							RenderTargetArrayIndex[MaxSimultaneousRenderTargets];
	FOpenGLTexture*					DepthStencil;
	ERenderTargetStoreAction		StencilStoreAction;
	uint32							DepthTargetWidth;
	uint32							DepthTargetHeight;
	bool							bFramebufferSetupInvalid;

	// we null this when the we dirty PackedGlobalUniformDirty. Thus we can skip all of CommitNonComputeShaderConstants if it matches the current program
	FOpenGLLinkedProgram* LinkedProgramAndDirtyFlag;
	FOpenGLShaderParameterCache*	ShaderParameters;

	TRefCountPtr<FOpenGLBoundShaderState>	BoundShaderState;
	TRefCountPtr<FOpenGLComputeShader>		CurrentComputeShader;

	/** The RHI does not allow more than 14 constant buffers per shader stage due to D3D11 limits. */
	enum { MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE = 14 };

	/** Track the currently bound uniform buffers. */
	FRHIUniformBuffer* BoundUniformBuffers[SF_NumStandardFrequencies][MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE];
	uint32 BoundUniformBuffersDynamicOffset[SF_NumStandardFrequencies][MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE];

	/** Array to track if any real (not emulated) uniform buffers have been bound since the last draw call */
	bool bAnyDirtyRealUniformBuffers[SF_NumStandardFrequencies];
	/** Bit array to track which uniform buffers have changed since the last draw call. */
	bool bAnyDirtyGraphicsUniformBuffers;
	uint16 DirtyUniformBuffers[SF_NumStandardFrequencies];

	// Used for if(!FOpenGL::SupportsFastBufferData())
	uint32 UpVertexBufferBytes;
	uint32 UpIndexBufferBytes;
	uint32 UpStride;
	void* UpVertexBuffer;
	void* UpIndexBuffer;
};

template<>
struct TOpenGLResourceTraits<FRHISamplerState>
{
    using TConcreteType = FOpenGLSamplerState;
};
template<>
struct TOpenGLResourceTraits<FRHIRasterizerState>
{
    using TConcreteType = FOpenGLRasterizerState;
};
template<>
struct TOpenGLResourceTraits<FRHIDepthStencilState>
{
    using TConcreteType = FOpenGLDepthStencilState;
};
template<>
struct TOpenGLResourceTraits<FRHIBlendState>
{
    using TConcreteType = FOpenGLBlendState;
};