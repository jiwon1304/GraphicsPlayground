#pragma once

#include "OpenGLThirdParty.h"
#include "RHI/RHIDefinitions.h"
#include "RHI/RHIResources.h"
#include "Container/Array.h"

enum
{
    OGL_MAX_UNIFORM_BUFFER_BINDINGS = 12,
    OGL_FIRST_UNIFORM_BUFFER = 0
};

class FOpenGLLinkedProgram;

/**
 * Base class for OpenGL shaders (vertex, pixel, etc.)
 */
class FOpenGLShader
{
public:
    FOpenGLShader(const TArray<const uint8>& Code, GLenum ShaderType, uint16 Hash);
    ~FOpenGLShader();

    uint16 GetHash() const { return Hash; }

    virtual bool Recompile(const TArray<const uint8>& Code) = 0;

    GLuint GetResource() const { return Resource; }
protected:
    static GLuint Compile(const TArray<const uint8>& Code, GLenum ShaderType);
    
    GLuint Resource = 0;
private:
    const uint16 Hash;
};

class FOpenGLVertexShader : public FRHIVertexShader, public FOpenGLShader
{
public:
    static constexpr EShaderType ShaderType = EShaderType::Vertex;

    FOpenGLVertexShader(const TArray<const uint8>& Code, uint16 Hash)
        : FRHIVertexShader(), FOpenGLShader(Code, GL_VERTEX_SHADER, Hash) {}

    virtual bool Recompile(const TArray<const uint8>& Code) override;
};

class FOpenGLPixelShader : public FRHIPixelShader, public FOpenGLShader
{
public:
    static constexpr EShaderType ShaderType = EShaderType::Pixel;

    FOpenGLPixelShader(const TArray<const uint8>& Code, uint16 Hash)
        : FRHIPixelShader(), FOpenGLShader(Code, GL_FRAGMENT_SHADER, Hash) {}

    virtual bool Recompile(const TArray<const uint8>& Code) override;
};

class FOpenGLGeometryShader : public FRHIGeometryShader, public FOpenGLShader
{
public:
    static constexpr EShaderType ShaderType = EShaderType::Geometry;

    FOpenGLGeometryShader(const TArray<const uint8>& Code, uint16 Hash)
        : FRHIGeometryShader(), FOpenGLShader(Code, GL_GEOMETRY_SHADER, Hash) {}

    virtual bool Recompile(const TArray<const uint8>& Code) override;
};

class FOpenGLComputeShader : public FRHIComputeShader, public FOpenGLShader
{
public:
    static constexpr EShaderType ShaderType = EShaderType::Compute;

    FOpenGLComputeShader(const TArray<const uint8>& Code, uint16 Hash)
        : FRHIComputeShader(), FOpenGLShader(Code, GL_NONE, Hash)
    {
        assert(false && "OpenGL 3.3 does not support compute shader");
    }
};