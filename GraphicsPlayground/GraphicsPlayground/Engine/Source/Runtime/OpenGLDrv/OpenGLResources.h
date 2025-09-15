#pragma once

#include "RHI/RHIResources.h"
#include "OpenGL3.h"
#include "RHI/RHICommandList.h"
#include "Core/HAL/PlatformMemory.h"

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
        }

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
    using TConcreteType = class FOpenGLBuffer;
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