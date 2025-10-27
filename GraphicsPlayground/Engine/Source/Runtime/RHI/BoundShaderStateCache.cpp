#include "BoundShaderStateCache.h"
#include "RHIResources.h"

/**
 * Simple cache for pointer
 */
TMap<FBoundShaderStateKey, FRHIBoundShaderState*> GBoundShaderStateCache;

/**
 * Returns a cached bound shader state.
 * return nullptr if not found.
 */
FRHIBoundShaderState* GetCachedBoundShaderState(
    FRHIVertexDeclaration* VertexDeclaration,
    FRHIVertexShader* VertexShader,
    FRHIPixelShader* PixelShader,
    FRHIGeometryShader* GeometryShader
)
{
    FBoundShaderStateKey Key(VertexDeclaration, VertexShader, PixelShader, GeometryShader);
    if (GBoundShaderStateCache.Contains(Key))
    {
        return GBoundShaderStateCache[Key];
    }

    return nullptr;
}

void AddBoundShaderStateToCache(
    FRHIVertexDeclaration* VertexDeclaration,
    FRHIVertexShader* VertexShader,
    FRHIPixelShader* PixelShader,
    FRHIGeometryShader* GeometryShader,
    FRHIBoundShaderState* BoundShaderState
)
{
    FBoundShaderStateKey Key(VertexDeclaration, VertexShader, PixelShader, GeometryShader);
    GBoundShaderStateCache.Add(Key, BoundShaderState);
}