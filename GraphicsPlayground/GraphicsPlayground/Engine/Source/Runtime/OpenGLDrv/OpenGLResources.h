#pragma once

#include "RHI/RHIResources.h"
#include "OpenGL3.h"
#include "RHI/RHICommandList.h"
#include "Core/HAL/PlatformMemory.h"
#include "Core/Math/Rect.h"

class FOpenGLLinkedProgram;
class FOpenGLVertexShader;
class FOpenGLPixelShader;
class FOpenGLGeometryShader;

#pragma region OpenGL Buffer
class FOpenGLBufferBase
{
protected:
	FOpenGLBufferBase(GLenum Type)
		: Type(Type)
	{}

public:
	GLenum Type;
	GLuint Resource = 0;

	void Bind();
	void OnBufferDeletion();
};

class FOpenGLView
{
public:
    void UpdateView()
    {
        if (Next)
        {
            /**
             * Update logic here (when needed)
             */
            Next->UpdateView();
        }
    }

protected:
    FOpenGLView* Next = nullptr;
};

/**
 * There is yet no case where a resource's state changes.
 * So we just track for GC.
 */
class FOpenGLViewableResource
{
public:
    ~FOpenGLViewableResource()
    {
        // assure we don't have any views left
        assert(LinkedViews);
    }
private:
    // the head of linked list of views
    FOpenGLView* LinkedViews = nullptr;
};

typedef void (BufferBindFunc)( GLenum target, GLuint buffer );

/**
 * class for common buffer properties, inherits "BaseType"
 * this type must be FRHIBuffer or derived class
 * for bind function, use template parameter "BufferBindFunc"
 */
// template </* typename BaseType, */ BufferBindFunc>
template <typename BaseType, BufferBindFunc>
class TOpenGLBuffer : public BaseType
{
public:
    GLuint Resource = 0;
    GLenum Type;
private:
    static_assert(std::is_base_of<FRHIBuffer, BaseType>::value, "BaseType must be FRHIBuffer or derived class");
    // void LoadData(uint32 InOffset, uint32 InSize, const void* InData)
    // {
    //     this->Bind();
    // }

    GLenum GetAccess()
    {
        return EnumHasAnyFlags(BaseType::GetUsage(), EBufferUsageFlags::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    }

public:
    TOpenGLBuffer(FRHICommandListBase* RHICmdList, GLenum InType, const FRHIBufferDesc& InDesc, const void* InData)
        : BaseType(InDesc)
        , Type(InType)
    {
        // @todo : alloc in command list
        uint32 Size = BaseType::GetSize();
        void* LocalCopy = nullptr;
        if ( InData )
        {
            LocalCopy = FPlatformMemory::Malloc<EAT_RHI>(Size);
            FPlatformMemory::Memcpy(LocalCopy, InData, Size);
        }
        auto InitLambda = [this, LocalCopy, Size]()
        {
            FOpenGL::GenBuffers(1, &this->Resource);
            this->Bind();
            FOpenGL::BufferData(this->Type, Size, LocalCopy, GetAccess());
            FPlatformMemory::Free(LocalCopy, Size);
        };
        
        if ( RHICmdList )
        {
            RHICmdList->EnqueueLambda(InitLambda);
        }
        else
        {
            InitLambda();
        }
    }

    void Bind()
    {
        BufferBindFunc(Type, Resource);
    }
};

// /**
//  * Since OpenGL separates common buffer and texture buffer, we have two classes.
//  */
// class FOpenGLBasePixelBuffer
// {
// public:
//     FOpenGLBasePixelBuffer(const FRHIBufferDesc& InDesc)
//         : Desc(InDesc)
//     {
//         FOpenGL::GenTextures(1, &Resource);
//         // glBindTexture(GL_TEXTURE_BUFFER, Resource);
//         // glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, Buffer);
//     }

//     FRHIBufferDesc Desc;
// };

class FOpenGLBaseBuffer : public FRHIBuffer, public FOpenGLViewableResource
{
public:
    FOpenGLBaseBuffer(const FRHIBufferDesc& InDesc)
        : FRHIBuffer(InDesc)
    {
    }

    ~FOpenGLBaseBuffer()
    {
    }
};

using FOpenGLBuffer = TOpenGLBuffer<FOpenGLBaseBuffer, FOpenGL::BindBuffer>;

class FOpenGLUniformBuffer : public FRHIUniformBuffer, public FOpenGLViewableResource
{
public:
    GLuint Resource;
    uint32 Size;

    FOpenGLUniformBuffer(const FRHIUniformBufferLayout* InLayout);

    ~FOpenGLUniformBuffer()
    {
        if (Resource)
        {
            FOpenGL::DeleteBuffers(1, &Resource);
            Resource = 0;
        }
    }

    // void SetGLUniformBufferParams()  

};
#pragma endregion OpenGL Buffer

#pragma region OpenGL VertexDeclaration
struct FOpenGLVertexElement
{
public:
    GLenum Type; // Primitive Type
    GLuint StreamIndex; // ??? same with FVertexElement::StreamIndex
    GLuint Offset; // offset in bytes within VAO
    GLuint Size; // Number of components
    uint8 bNormalized; // normalized or not
    uint8 AttributeIndex; // attribute index in shader
    uint16 Padding;
};

using FOpenGLVertexElements = TArray<
    FOpenGLVertexElement, 
    TSizedInlineAllocator<FOpenGLVertexElement, MaxVertexElementCount>
    >;

/**
 * A set of vertex elements.
 */
class FOpenGLVertexDeclaration : public FRHIVertexDeclaration
{
public:
    FOpenGLVertexElements VertexElements;

    FOpenGLVertexDeclaration(const FOpenGLVertexElements& InElements)
        : FRHIVertexDeclaration()
        , VertexElements(InElements)
    {
    }

    virtual bool GetInitializer(FVertexDeclarationElementList& Init) override;
};
#pragma endregion OpenGL VertexDeclaration

#pragma region OpenGL Shader
/**
 * Contains vertex elements and shaders.
 * Other buffers are not specified here.
 */
class FOpenGLBoundShaderState : public FRHIBoundShaderState
{
    static FOpenGLLinkedProgram* FindOrCreateLinkedProgram(
        FOpenGLVertexShader* VertexShader, 
        FOpenGLPixelShader* PixelShader, 
        FOpenGLGeometryShader* GeometryShader
    );

public:
    FOpenGLBoundShaderState(
        FOpenGLVertexDeclaration* InVertexDeclaration,
        FOpenGLVertexShader* InVertexShader,
        FOpenGLPixelShader* InPixelShader,
        FOpenGLGeometryShader* InGeometryShader
    )
        : FRHIBoundShaderState()
        , VertexDeclaration(InVertexDeclaration)
        , VertexShader(InVertexShader)
        , PixelShader(InPixelShader)
        , GeometryShader(InGeometryShader)
    {
        assert(VertexDeclaration && VertexShader && PixelShader);
        LinkedProgram = FindOrCreateLinkedProgram(VertexShader, PixelShader, GeometryShader);
    }

    const FOpenGLLinkedProgram* LinkedProgram;
    TRefCountPtr<FOpenGLVertexDeclaration> VertexDeclaration;
    TRefCountPtr<FOpenGLVertexShader> VertexShader;
    TRefCountPtr<FOpenGLPixelShader> PixelShader;
    TRefCountPtr<FOpenGLGeometryShader> GeometryShader;
};
#pragma endregion OpenGL Shader

#pragma region OpenGL Texture
class FOpenGLTextureDesc
{
public:
    FOpenGLTextureDesc(FRHITextureDesc const& InDesc);

    uint32 MemorySize = 0;
};

class FOpenGLTextureCreateDesc : public FRHITextureCreateDesc, public FOpenGLTextureDesc
{
public:
	FOpenGLTextureCreateDesc(FRHITextureCreateDesc const& CreateDesc)
		: FRHITextureCreateDesc(CreateDesc)
		, FOpenGLTextureDesc(CreateDesc)
	{
	}
};

class FOpenGLTexture : public FRHITexture, public FOpenGLViewableResource
{
	// Copy and assignment are disabled
	FOpenGLTexture(FOpenGLTexture const&) = delete;
	FOpenGLTexture& operator = (FOpenGLTexture const&) = delete;

public:
	// Standard constructor
	explicit FOpenGLTexture(FRHICommandListBase& RHICmdList, FOpenGLTextureCreateDesc const& CreateDesc);

	virtual ~FOpenGLTexture();

	virtual void* GetTextureBaseRHI() override final
	{
		return this;
	}

    // OpenGL does not use pointer
	virtual void* GetNativeResource() const override
	{
        return nullptr;
	}

	void DeleteGLResource();

private:
	/** The OpenGL texture resource. */
	GLuint Resource = GL_NONE;

public:
    // The texture target (GL_TEXTURE_2D, etc)
	GLenum const Target = 0;

    // The attachment point for this texture when used as a render target.
	GLenum const Attachment = 0;

	// Pointer to current sampler state in this unit
	class FOpenGLSamplerState* SamplerState = nullptr;

public:
	uint32 const MemorySize; // in case of memory tracking
};
#pragma endregion OpenGL Texture

#pragma region OpenGL UAV SRV
class FOpenGLUnorderedAccessView final : public FRHIUnorderedAccessView, public FOpenGLView
{
public:
	FOpenGLUnorderedAccessView(FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc);
	virtual ~FOpenGLUnorderedAccessView();

	FOpenGLViewableResource* GetBaseResource() const;

	// void UpdateView() override;

    // Actual GL resource
	GLuint Resource = 0;
	GLuint BufferResource = 0;
	GLenum Format = 0;

	uint32 GetBufferSize() const
	{
		return IsBuffer() ? GetBuffer()->GetSize() : 0;
	}
};

class FOpenGLShaderResourceView final : public FRHIShaderResourceView, public FOpenGLView
{
public:
	FOpenGLShaderResourceView(FRHICommandListBase& RHICmdList, FRHIViewableResource* Resource, FRHIViewDesc const& ViewDesc);
	virtual ~FOpenGLShaderResourceView();

	FOpenGLViewableResource* GetBaseResource() const;

	// void UpdateView() override;

	/** OpenGL texture the buffer is bound with */
	GLuint Resource = GL_NONE;
	GLenum Target = GL_TEXTURE_BUFFER;

	int32 LimitMip = -1;

private:
	void Invalidate();
	bool OwnsResource = false;
};

#pragma endregion OpenGL UAV SRV

#pragma region OpenGL Viewport
class FOpenGLViewport : public FRHIViewport
{
public:
	FOpenGLViewport(class FOpenGLDynamicRHI* InOpenGLRHI,
        void* InWindowHandle,
        uint32 InSizeX, uint32 InSizeY,
        bool bInIsFullscreen, EPixelFormat PreferredPixelFormat);
	~FOpenGLViewport();

	void Resize(uint32 InSizeX,uint32 InSizeY,bool bInIsFullscreen);

	// Accessors.
	FIntPoint GetSizeXY() const { return FIntPoint(SizeX, SizeY); }
	FOpenGLTexture* GetBackBuffer() const { return BackBuffer; }
	bool IsFullscreen() const { return bIsFullscreen; }

	virtual void WaitForFrameEventCompletion() override;
	virtual void IssueFrameEvent() override;

	virtual void* GetNativeWindow() const override { return nullptr; } // @todo : implement

	struct FPlatformOpenGLContext* GetGLContext() const { return OpenGLContext; }
	FOpenGLDynamicRHI* GetOpenGLRHI() const { return OpenGLRHI; }

private:
	friend class FOpenGLDynamicRHI;

	FOpenGLDynamicRHI* OpenGLRHI;
	struct FPlatformOpenGLContext* OpenGLContext;
	uint32 SizeX;
	uint32 SizeY;
	bool bIsFullscreen;
	EPixelFormat PixelFormat;
	bool bIsValid;
	TRefCountPtr<FOpenGLTexture> BackBuffer;
};

#pragma endregion OpenGL Viewport


/**
 * We use template (traits) to map RHI resource types to OpenGL resource types.
 * With traits, we can cast FRHI* to FOpenGL* in compile time.
 */
template<class T>
struct TOpenGLResourceTraits
{
};
template<>
struct TOpenGLResourceTraits<FRHISamplerState>
{
    using TConcreteType = class FOpenGLSamplerState;
};
template<>
struct TOpenGLResourceTraits<FRHIRasterizerState>
{
    using TConcreteType = class FOpenGLRasterizerState;
};
template<>
struct TOpenGLResourceTraits<FRHIDepthStencilState>
{
    using TConcreteType = class FOpenGLDepthStencilState;
};
template<>
struct TOpenGLResourceTraits<FRHIBlendState>
{
    using TConcreteType = class FOpenGLBlendState;
};
template<>
struct TOpenGLResourceTraits<FRHIVertexDeclaration>
{
    using TConcreteType = class FOpenGLVertexDeclaration;
};
template<>
struct TOpenGLResourceTraits<FRHIBoundShaderState>
{
    using TConcreteType = class FOpenGLBoundShaderState;
};
template<>
struct TOpenGLResourceTraits<FRHIVertexShader>
{
    using TConcreteType = class FOpenGLVertexShader;
};
template<>
struct TOpenGLResourceTraits<FRHIPixelShader>
{
    using TConcreteType = class FOpenGLPixelShader;
};
template<>
struct TOpenGLResourceTraits<FRHIGeometryShader>
{
    using TConcreteType = class FOpenGLGeometryShader;
};
template<>
struct TOpenGLResourceTraits<FRHIComputeShader>
{
    using TConcreteType = class FOpenGLComputeShader;
};
template<>
struct TOpenGLResourceTraits<FRHIUniformBuffer>
{
    using TConcreteType = class FOpenGLUniformBuffer;
};
template<>
struct TOpenGLResourceTraits<FRHIBuffer>
{
    using TConcreteType = FOpenGLBuffer;
};
template<>
struct TOpenGLResourceTraits<FRHIViewport>
{
    using TConcreteType = class FOpenGLViewport;
};
template<>
struct TOpenGLResourceTraits<FRHIUnorderedAccessView>
{
    using TConcreteType = class FOpenGLUnorderedAccessView;
};
template<>
struct TOpenGLResourceTraits<FRHIShaderResourceView>
{
    using TConcreteType = class FOpenGLShaderResourceView;
};
template<>
struct TOpenGLResourceTraits<FRHIStagingBuffer>
{
    using TConcreteType = class FOpenGLStagingBuffer;
};