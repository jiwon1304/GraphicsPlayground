/**
 * In OpenGL, shaders are compiled in a form of "program".
 * Thus we have to link each shader before using them.
 */
#include "OpenGLResources.h"
#include "OpenGLShaderResources.h"
#include "OpenGLDrv.h"
#include "Core/Container/Map.h"
#include "OpenGL3.h"
#include "BoundShaderStateCache.h"

TMap<uint16, FOpenGLShader*> GOpenGLCompiledShaderCache;
TMap<uint64, FOpenGLLinkedProgram*> GOpenGLLinkedProgramCache;

void DestroyShadersAndPrograms()
{
    for (auto& Pair : GOpenGLCompiledShaderCache)
    {
        FOpenGLShader* Shader = Pair.Value;
        delete Shader;
    }
    GOpenGLCompiledShaderCache.GetContainerPrivate().clear();

    for (auto& Pair : GOpenGLLinkedProgramCache)
    {
        FOpenGLLinkedProgram* Program = Pair.Value;
        delete Program;
    }
    GOpenGLLinkedProgramCache.GetContainerPrivate().clear();
}

class FOpenGLLinkedProgram
{
public:
    static FOpenGLLinkedProgram* FindOrCreate(
        FOpenGLVertexShader* VertexShader,
        FOpenGLPixelShader* PixelShader,
        FOpenGLGeometryShader* GeometryShader = nullptr
    );

    GLuint Resource = 0;
    
    ~FOpenGLLinkedProgram()
    {
        if (Resource)
        {
            FOpenGL::DeleteProgram(Resource);
        }
    }
    
private:
    FOpenGLLinkedProgram(FOpenGLVertexShader* VertexShader, FOpenGLPixelShader* PixelShader, FOpenGLGeometryShader* GeometryShader = nullptr);
    
    // left 16 bits are empty. We use only 48 bits : vertex - pixel - geometry from left to right
    static uint64 ConcatHash(uint16 VertexHash, uint16 PixelHash, uint16 GeometryHash)
    {
        return (static_cast<uint64>(VertexHash) << 32) | (static_cast<uint64>(PixelHash) << 16) | static_cast<uint64>(GeometryHash);
    }
};

FOpenGLShader::FOpenGLShader(const TArray<const uint8> &Code, GLenum ShaderType, uint16 Hash)
    : Hash(Hash)
{
    if (GOpenGLCompiledShaderCache.Contains(Hash))
    {
        Resource = GOpenGLCompiledShaderCache[Hash]->Resource;
        return;
    }

    Resource = Compile(Code, ShaderType);

    if(Resource == 0)
    {
        std::fprintf(stderr, "Failed to compile shader with hash %u\n", Hash);
    }
}

FOpenGLShader::~FOpenGLShader()
{
    if (Resource)
    {
        FOpenGL::DeleteShader(Resource);
    }
}

GLuint FOpenGLShader::Compile(const TArray<const uint8>& Code, GLenum ShaderType)
{
    const char* ShaderCode = reinterpret_cast<const char*>(Code.GetData());
    assert(ShaderCode);

    return FOpenGL::CreateShader(ShaderType, ShaderCode);
}

bool FOpenGLVertexShader::Recompile(const TArray<const uint8>& Code)
{
    assert(Resource != 0);

    FOpenGL::DeleteShader(Resource);

    Resource = Compile(Code, GL_VERTEX_SHADER);
    return Resource != 0;
}

bool FOpenGLPixelShader::Recompile(const TArray<const uint8>& Code)
{
    assert(Resource != 0);

    FOpenGL::DeleteShader(Resource);

    Resource = Compile(Code, GL_FRAGMENT_SHADER);
    return Resource != 0;
}

bool FOpenGLGeometryShader::Recompile(const TArray<const uint8>& Code)
{
    assert(Resource != 0);

    FOpenGL::DeleteShader(Resource);

    Resource = Compile(Code, GL_GEOMETRY_SHADER);
    return Resource != 0;
}

FOpenGLLinkedProgram *FOpenGLLinkedProgram::FindOrCreate(FOpenGLVertexShader *VertexShader, FOpenGLPixelShader *PixelShader, FOpenGLGeometryShader *GeometryShader)
{
    assert(VertexShader && PixelShader);

    uint64 Hash = ConcatHash(VertexShader->GetHash(), PixelShader->GetHash(), GeometryShader ? GeometryShader->GetHash() : 0);
    if (GOpenGLLinkedProgramCache.Contains(Hash))
    {
        return GOpenGLLinkedProgramCache[Hash];
    }

    FOpenGLLinkedProgram* NewProgram = new FOpenGLLinkedProgram(VertexShader, PixelShader, GeometryShader);
    GOpenGLLinkedProgramCache.Add(Hash, NewProgram);
    return NewProgram;
}

FOpenGLLinkedProgram::FOpenGLLinkedProgram(FOpenGLVertexShader *VertexShader, FOpenGLPixelShader *PixelShader, FOpenGLGeometryShader *GeometryShader)
{
    Resource = FOpenGL::CreateProgram(
        VertexShader->GetResource(), PixelShader->GetResource(),
        GeometryShader ? GeometryShader->GetResource() : 0);

    if (Resource == 0)
    {
        std::fprintf(stderr, "Failed to create program\n");
        return;
    }
}

FOpenGLLinkedProgram* FOpenGLBoundShaderState::FindOrCreateLinkedProgram(FOpenGLVertexShader* VertexShader, FOpenGLPixelShader* PixelShader, FOpenGLGeometryShader* GeometryShader)
{
    return FOpenGLLinkedProgram::FindOrCreate(VertexShader, PixelShader, GeometryShader);
}

FPixelShaderRHIRef FOpenGLDynamicRHI::RHICreatePixelShader(const TArray<const uint8> &Code, const uint16 Hash)
{
    return new FOpenGLPixelShader(Code, Hash);
}

FVertexShaderRHIRef FOpenGLDynamicRHI::RHICreateVertexShader(const TArray<const uint8> &Code, const uint16 Hash)
{
    return FVertexShaderRHIRef();
}

FGeometryShaderRHIRef FOpenGLDynamicRHI::RHICreateGeometryShader(const TArray<const uint8> &Code, const uint16 Hash)
{
    return FGeometryShaderRHIRef();
}

FBoundShaderStateRHIRef FOpenGLDynamicRHI::RHICreateBoundShaderState(
    FRHIVertexDeclaration *VertexDeclarationRHI,
    FRHIVertexShader *VertexShaderRHI,
    FRHIPixelShader *PixelShaderRHI,
    FRHIGeometryShader *GeometryShaderRHI)
{
    FOpenGLVertexDeclaration* VertexDeclaration = FOpenGLDynamicRHI::ResourceCast(VertexDeclarationRHI);
    FOpenGLVertexShader* VertexShader = FOpenGLDynamicRHI::ResourceCast(VertexShaderRHI);
    FOpenGLPixelShader* PixelShader = FOpenGLDynamicRHI::ResourceCast(PixelShaderRHI);
    FOpenGLGeometryShader* GeometryShader = FOpenGLDynamicRHI::ResourceCast(GeometryShaderRHI);

    FRHIBoundShaderState* BoundShaderState = GetCachedBoundShaderState(
        VertexDeclarationRHI, VertexShaderRHI, PixelShaderRHI, GeometryShaderRHI);

    if (BoundShaderState)
    {
        return BoundShaderState;
    }
    else
    {
        BoundShaderState = new FOpenGLBoundShaderState(
            VertexDeclaration, VertexShader, PixelShader, GeometryShader);
        AddBoundShaderStateToCache(
            VertexDeclarationRHI, VertexShaderRHI, PixelShaderRHI, GeometryShaderRHI,
            BoundShaderState);
        return BoundShaderState;
    }
}
