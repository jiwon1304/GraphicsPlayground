#include "OpenGLDrv.h"

#include "Core/Container/Map.h"
#include "OpenGLResources.h"

// @todo : tarray as key?
TMap<FVertexDeclarationElementList, FVertexDeclarationRHIRef> GOpenGLVertexDeclarationCache;

FVertexDeclarationRHIRef FOpenGLDynamicRHI::RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements)
{
    // Check if the vertex declaration already exists in the cache
    FVertexDeclarationRHIRef* VertexDeclaration = GOpenGLVertexDeclarationCache.Find(Elements);
    if (VertexDeclaration)
    {
        return *VertexDeclaration;
    }

    // Create OpenGL vertex elements from the provided elements
    FOpenGLVertexElements GLVertexElements;
    for (const FVertexElement& Element : Elements)
    {
        FOpenGLVertexElement GLElement;
        GLElement.StreamIndex = Element.StreamIndex;
        GLElement.Offset = Element.Offset;
        GLElement.AttributeIndex = Element.AttributeIndex;
        GLElement.bNormalized = 0; // Assuming not normalized, modify as needed

        switch (Element.Type)
        {
            case EVertexElementType::Float:
                GLElement.Type = GL_FLOAT;
                GLElement.Size = 1;
                break;
            case EVertexElementType::Float2:
                GLElement.Type = GL_FLOAT;
                GLElement.Size = 2;
                break;
            case EVertexElementType::Float3:
                GLElement.Type = GL_FLOAT;
                GLElement.Size = 3;
                break;
            case EVertexElementType::Float4:
                GLElement.Type = GL_FLOAT;
                GLElement.Size = 4;
                break;
            case EVertexElementType::Int:
                GLElement.Type = GL_INT;
                GLElement.Size = 1;
                break;
            case EVertexElementType::Int2:
                GLElement.Type = GL_INT;
                GLElement.Size = 2;
                break;
            case EVertexElementType::Int3:
                GLElement.Type = GL_INT;
                GLElement.Size = 3;
                break;
            case EVertexElementType::Int4:
                GLElement.Type = GL_INT;
                GLElement.Size = 4;
                break;
            case EVertexElementType::UInt:
                GLElement.Type = GL_UNSIGNED_INT;
                GLElement.Size = 1;
                break;
            case EVertexElementType::UInt2:
                GLElement.Type = GL_UNSIGNED_INT;
                GLElement.Size = 2;
                break;
            case EVertexElementType::UInt3:
                GLElement.Type = GL_UNSIGNED_INT;
                GLElement.Size = 3;
                break;
            case EVertexElementType::UInt4:
                GLElement.Type = GL_UNSIGNED_INT;
                GLElement.Size = 4;
                break;
            case EVertexElementType::Byte:
                GLElement.Type = GL_BYTE;
                GLElement.Size = 1; // Assuming single byte
                break;
            case EVertexElementType::Short:
                GLElement.Type = GL_SHORT;
                GLElement.Size = 1; // Assuming single short
                break;
            default:
                assert(false && "Unsupported vertex element type");
                break;
        }
    }

    // Create a new vertex declaration
    FVertexDeclarationRHIRef NewDecl = 
        static_cast<FOpenGLVertexDeclaration*>(FPlatformMemory::Malloc<EAT_RHI>(sizeof(FOpenGLVertexDeclaration)));
    new (NewDecl.GetReference()) FOpenGLVertexDeclaration(GLVertexElements);

    // Cache the new vertex declaration
    GOpenGLVertexDeclarationCache.Add(Elements, NewDecl);

    return NewDecl;
}

bool FOpenGLVertexDeclaration::GetInitializer(FVertexDeclarationElementList& Init)
{
    for (const FOpenGLVertexElement& GLElement : VertexElements)
    {
        FVertexElement Element;
        Element.StreamIndex = static_cast<uint8>(GLElement.StreamIndex);
        Element.Offset = static_cast<uint8>(GLElement.Offset);
        Element.AttributeIndex = GLElement.AttributeIndex;

        switch (GLElement.Type)
        {
            case GL_FLOAT:
                switch (GLElement.Size)
                {
                    case 1: Element.Type = EVertexElementType::Float; break;
                    case 2: Element.Type = EVertexElementType::Float2; break;
                    case 3: Element.Type = EVertexElementType::Float3; break;
                    case 4: Element.Type = EVertexElementType::Float4; break;
                    default: return false; // Invalid size
                    Element.Stride = static_cast<uint16>(GLElement.Size * sizeof(float));
                }
                break;
            case GL_INT:
                switch (GLElement.Size)
                {
                    case 1: Element.Type = EVertexElementType::Int; break;
                    case 2: Element.Type = EVertexElementType::Int2; break;
                    case 3: Element.Type = EVertexElementType::Int3; break;
                    case 4: Element.Type = EVertexElementType::Int4; break;
                    default: return false; // Invalid size
                }
                Element.Stride = static_cast<uint16>(GLElement.Size * sizeof(int32));
                break;
            case GL_UNSIGNED_INT:
                switch (GLElement.Size)
                {
                    case 1: Element.Type = EVertexElementType::UInt; break;
                    case 2: Element.Type = EVertexElementType::UInt2; break;
                    case 3: Element.Type = EVertexElementType::UInt3; break;
                    case 4: Element.Type = EVertexElementType::UInt4; break;
                    default: return false; // Invalid size
                }
                Element.Stride = static_cast<uint16>(GLElement.Size * sizeof(uint32));
                break;
            case GL_BYTE:
                if (GLElement.Size == 1) Element.Type = EVertexElementType::Byte;
                else return false; // Invalid size
                Element.Stride = static_cast<uint16>(sizeof(int8));
                break;
            case GL_SHORT:
                if (GLElement.Size == 1) Element.Type = EVertexElementType::Short;
                else return false; // Invalid size
                Element.Stride = static_cast<uint16>(sizeof(int16));
                break;
            default:
                return false; // Unsupported type
        }
    }
}