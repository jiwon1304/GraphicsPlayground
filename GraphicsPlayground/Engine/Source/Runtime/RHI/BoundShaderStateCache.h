#pragma once

#include "RHI.h"
#include "Core/Container/Map.h"

class FBoundShaderStateKey
{
    FRHIVertexDeclaration* VertexDeclaration;
    FRHIVertexShader* VertexShader;
    FRHIPixelShader* PixelShader;
    FRHIGeometryShader* GeometryShader;
public:
    /**
     * Pointers should be unique.
     */
    FBoundShaderStateKey(
        FRHIVertexDeclaration* InVertexDeclaration,
        FRHIVertexShader* InVertexShader,
        FRHIPixelShader* InPixelShader,
        FRHIGeometryShader* InGeometryShader)
        : VertexDeclaration(InVertexDeclaration)
        , VertexShader(InVertexShader)
        , PixelShader(InPixelShader)
        , GeometryShader(InGeometryShader)
    {}

private:
    uint64 GetHash() const
    {
        return
            (static_cast<uint64>(reinterpret_cast<uintptr_t>(VertexDeclaration))) ^
            (static_cast<uint64>(reinterpret_cast<uintptr_t>(VertexShader))) ^
            (static_cast<uint64>(reinterpret_cast<uintptr_t>(PixelShader))) ^
            (static_cast<uint64>(reinterpret_cast<uintptr_t>(GeometryShader)));
    }

    friend bool operator==(const FBoundShaderStateKey& Lhs, const FBoundShaderStateKey& Rhs)
    {
        return Lhs.VertexDeclaration == Rhs.VertexDeclaration
            && Lhs.VertexShader == Rhs.VertexShader
            && Lhs.PixelShader == Rhs.PixelShader
            && Lhs.GeometryShader == Rhs.GeometryShader;
    }

    friend struct std::hash<FBoundShaderStateKey>;
};

namespace std
{
    template<>
    struct hash<FBoundShaderStateKey>
    {
        size_t operator()(const FBoundShaderStateKey& Key) const
        {
            return static_cast<size_t>(Key.GetHash());
        }
    };
}

extern FRHIBoundShaderState* GetCachedBoundShaderState(
    FRHIVertexDeclaration* VertexDeclaration,
    FRHIVertexShader* VertexShader,
    FRHIPixelShader* PixelShader,
    FRHIGeometryShader* GeometryShader
);

extern void AddBoundShaderStateToCache(
    FRHIVertexDeclaration* VertexDeclaration,
    FRHIVertexShader* VertexShader,
    FRHIPixelShader* PixelShader,
    FRHIGeometryShader* GeometryShader,
    FRHIBoundShaderState* BoundShaderState
);