#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <dxgi.h>

#include "Windows/D3D11RHI/D3D11Resources.h"
#include <RHI/RHIResources.h>

class FD3D11DynamicRHI;

struct FD3D11Viewport : public FRHIViewport
{
protected:
    TSharedPtr<FD3D11DynamicRHI> D3DRHI;
    TSharedPtr<IDXGISwapChain> SwapChain;
    TSharedPtr<FD3D11Texture> BackBuffer;
    //D3D11_VIEWPORT Viewport;
public:
    FD3D11Viewport(TSharedPtr<FD3D11DynamicRHI> InD3D11RHI, FRHIViewportDesc InDesc);

    //FD3D11Viewport(IDXGISwapChain* InSwapChain, ID3D11RenderTargetView* InRTV, ID3D11Texture2D* InBackBuffer, ID3D11Texture2D* InDSVTex, ID3D11DepthStencilView* InDSV, FRHIViewportDesc InDesc)
    //    : FRHIViewport(InDesc)
    //    , SwapChain(InSwapChain)
    //    , BackBufferRTV(InRTV)
    //    , BackBufferTexture(InBackBuffer)
    //    , DepthStencilTexture(InDSVTex)
    //    , DepthStencilView(InDSV)
    //{
    //    Viewport.TopLeftX = 0.0f;
    //    Viewport.TopLeftY = 0.0f;
    //    Viewport.Width = static_cast<FLOAT>(Desc.Width);
    //    Viewport.Height = static_cast<FLOAT>(Desc.Height);
    //    Viewport.MinDepth = 0.0f;
    //    Viewport.MaxDepth = 1.0f;
    //}

    bool Present();

    virtual void Release() override final
    {
        if (SwapChain)
        {
            SwapChain->Release();
            SwapChain = nullptr;
        }
        if (BackBuffer)
        {
            BackBuffer->Release();
            BackBuffer = nullptr;
        }
    }
};
