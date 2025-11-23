#pragma once
#include <memory>
#include "Texture.h"
#include "Container/Map.h"

class ID3D11Device;
class ID3D11DeviceContext;
class FTexture;
#ifndef HRESULT 
typedef long HRESULT;
#endif

class FRenderer;
class FGraphicsDevice;
class FResourceManager
{

public:
    void Initialize(FRenderer* Renderer, FGraphicsDevice* Device);
    void Release(FRenderer* Renderer);
    HRESULT LoadTextureFromFile(/* ID3D11Device* Device,  */const wchar_t* Filename, bool bIsSRGB = true);
    HRESULT LoadTextureFromDDS(ID3D11Device* Device, ID3D11DeviceContext* Context, const wchar_t* Filename);

    FTexture* GetTexture(const FWString& Name) const;
private:
    TMap<FWString, FTexture*> TextureMap;
};
