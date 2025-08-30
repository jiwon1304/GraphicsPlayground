#pragma once

#include "RHI/RHIResources.h"

#define _TCHAR_DEFINED
#include "Define.h"
#include <d3d11.h>
#include <d3dcompiler.h>

namespace RHI
{
struct FD3D11Buffer : public FRHIBuffer
{
public:
    ID3D11Buffer* Resource;

    FD3D11Buffer(ID3D11Buffer* InResource, FRHIBufferDesc InDesc)
        : FRHIBuffer(InDesc)
        , Resource(InResource)
    {

    }

    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11Layout : public FRHIInputLayout
{
public:
    ID3D11InputLayout* Resource;

    FD3D11Layout(ID3D11InputLayout* InResource)
        : Resource(InResource)
    {

    }

    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11VertexShader : public FRHIVertexShader
{
public:
    ID3D11VertexShader* Resource;
    FD3D11VertexShader(ID3D11VertexShader* InResource, FRHIShaderDesc InDesc)
        : FRHIVertexShader(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11PixelShader : public FRHIPixelShader
{
public:
    ID3D11PixelShader* Resource;
    FD3D11PixelShader(ID3D11PixelShader* InResource, FRHIShaderDesc InDesc)
        : FRHIPixelShader(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11ComputeShader : public FRHIComputeShader
{
public:
    ID3D11ComputeShader* Resource;
    FD3D11ComputeShader(ID3D11ComputeShader* InResource, FRHIShaderDesc InDesc)
        : FRHIComputeShader(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11GeometryShader : public FRHIGeometryShader
{
public:
    ID3D11GeometryShader* Resource;
    FD3D11GeometryShader(ID3D11GeometryShader* InResource, FRHIShaderDesc InDesc)
        : FRHIGeometryShader(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11View : public FRHIView
{
    union
    {
        ID3D11ShaderResourceView* SRV;
        ID3D11RenderTargetView* RTV;
        ID3D11DepthStencilView* DSV;
        ID3D11UnorderedAccessView* UAV;
        void* Ptr;
    } Resource;

    virtual void Release() override final
    {
        if (Resource.Ptr)
        {
            if (ViewTarget == EViewTarget::SRV)
            {
                Resource.SRV->Release();
            }
            else if (ViewTarget == EViewTarget::RTV)
            {
                Resource.RTV->Release();
            }
            else if (ViewTarget == EViewTarget::DSV)
            {
                Resource.DSV->Release();
            }
            else if (ViewTarget == EViewTarget::UAV)
            {
                Resource.UAV->Release();
            }
        }
    }
};

struct FD3D11Texture : public FRHITexture
{
public:
    ID3D11Texture2D* Resource;

    FD3D11Texture(ID3D11Texture2D* InResource, ID3D11ShaderResourceView* InSRV, ID3D11RenderTargetView* InRTV, ID3D11DepthStencilView* InDSV, ID3D11UnorderedAccessView* InUAV, FRHITextureDesc InDesc)
        : FRHITexture(InDesc)
        , Resource(InResource)
    {
        if (InSRV)
        {
            FD3D11View* NewSRV = new FD3D11View();
            NewSRV->ViewTarget = EViewTarget::SRV;
            NewSRV->ViewType = static_cast<EViewType>(Desc.Type);
            NewSRV->Format = Desc.Format;
            NewSRV->MostDetailedMip = Desc.MostDetailedMip;
            NewSRV->MipLevels = Desc.MipLevels;
            NewSRV->Resource.SRV = InSRV;
            SRV = NewSRV;
        }

        if (InRTV)
        {
            FD3D11View* NewRTV = new FD3D11View();
            NewRTV->ViewTarget = EViewTarget::RTV;
            NewRTV->ViewType = static_cast<EViewType>(Desc.Type);
            NewRTV->Format = Desc.Format;
            NewRTV->MostDetailedMip = Desc.MostDetailedMip;
            NewRTV->MipLevels = Desc.MipLevels;
            NewRTV->Resource.RTV = InRTV;
            RTV = NewRTV;
        }

        if (InDSV)
        {
            FD3D11View* NewDSV = new FD3D11View();
            NewDSV->ViewTarget = EViewTarget::DSV;
            NewDSV->ViewType = static_cast<EViewType>(Desc.Type);
            NewDSV->Format = Desc.Format;
            NewDSV->MostDetailedMip = Desc.MostDetailedMip;
            NewDSV->MipLevels = Desc.MipLevels;
            NewDSV->Resource.DSV = InDSV;
            DSV = NewDSV;
        }

        if (InUAV)
        {
            FD3D11View* NewUAV = new FD3D11View();
            NewUAV->ViewTarget = EViewTarget::UAV;
            NewUAV->ViewType = static_cast<EViewType>(Desc.Type);
            NewUAV->Format = Desc.Format;
            NewUAV->MostDetailedMip = Desc.MostDetailedMip;
            NewUAV->MipLevels = Desc.MipLevels;
            NewUAV->Resource.UAV = InUAV;
            UAV = NewUAV;
        }
    }

    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
        if (SRV)
        {
            delete SRV;
            SRV = nullptr;
        }
        if (RTV)
        {
            delete RTV;
            RTV = nullptr;
        }
        if (DSV)
        {
            delete DSV;
            DSV = nullptr;
        }
        if (UAV)
        {
            delete UAV;
            UAV = nullptr;
        }
    }
};

struct FD3D11SamplerState : public FRHISamplerState
{
public:
    ID3D11SamplerState* Resource;
    FD3D11SamplerState(ID3D11SamplerState* InResource, FRHISamplerStateDesc InDesc)
        : FRHISamplerState(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11RasterizerState : public FRHIRasterizerState
{
public:
    ID3D11RasterizerState* Resource;
    FD3D11RasterizerState(ID3D11RasterizerState* InResource, FRHIRasterizerStateDesc InDesc)
        : FRHIRasterizerState(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11BlendState : public FRHIBlendState
{
public:
    ID3D11BlendState* Resource;
    FD3D11BlendState(ID3D11BlendState* InResource, FRHIBlendStateDesc InDesc)
        : FRHIBlendState(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11DepthStencilState : public FRHIDepthStencilState
{
public:
    ID3D11DepthStencilState* Resource;
    FD3D11DepthStencilState(ID3D11DepthStencilState* InResource, FRHIDepthStencilStateDesc InDesc)
        : FRHIDepthStencilState(InDesc)
        , Resource(InResource)
    {
    }
    virtual void Release() override final
    {
        if (Resource)
        {
            Resource->Release();
            Resource = nullptr;
        }
    }
};

struct FD3D11Viewport : public FRHIViewport
{
public:
    IDXGISwapChain* SwapChain;
    ID3D11RenderTargetView* BackBufferRTV;
    ID3D11Texture2D* BackBufferTexture;
    ID3D11Texture2D* DepthStencilTexture;
    ID3D11DepthStencilView* DepthStencilView;
    D3D11_VIEWPORT Viewport;
    FD3D11Viewport(IDXGISwapChain* InSwapChain, ID3D11RenderTargetView* InRTV, ID3D11Texture2D* InBackBuffer, ID3D11Texture2D* InDSVTex, ID3D11DepthStencilView* InDSV, FRHIViewportDesc InDesc)
        : FRHIViewport(InDesc)
        , SwapChain(InSwapChain)
        , BackBufferRTV(InRTV)
        , BackBufferTexture(InBackBuffer)
        , DepthStencilTexture(InDSVTex)
        , DepthStencilView(InDSV)
    {
        Viewport.TopLeftX = 0.0f;
        Viewport.TopLeftY = 0.0f;
        Viewport.Width = static_cast<FLOAT>(Desc.Width);
        Viewport.Height = static_cast<FLOAT>(Desc.Height);
        Viewport.MinDepth = 0.0f;
        Viewport.MaxDepth = 1.0f;
    }
    virtual void Release() override final
    {
        if (BackBufferRTV)
        {
            BackBufferRTV->Release();
            BackBufferRTV = nullptr;
        }
        if (BackBufferTexture)
        {
            BackBufferTexture->Release();
            BackBufferTexture = nullptr;
        }
        if (DepthStencilView)
        {
            DepthStencilView->Release();
            DepthStencilView = nullptr;
        }
        if (DepthStencilTexture)
        {
            DepthStencilTexture->Release();
            DepthStencilTexture = nullptr;
        }
        if (SwapChain)
        {
            SwapChain->Release();
            SwapChain = nullptr;
        }
    }
};

}; // namespace RHI
