#pragma once

#include "RHI/RHIResources.h"
#include "Define.h"

struct FD3D11Buffer : public RHI::FRHIBuffer
{
protected:
    ID3D11Buffer* Resource;

public:
    FD3D11Buffer(ID3D11Buffer* InResource, RHI::FRHIBufferDesc InDesc)
        : RHI::FRHIBuffer(InDesc)
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

struct FD3D11Layout : public RHI::FRHIInputLayout
{
protected:
    ID3D11InputLayout* Resource;

public:
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

struct FD3D11VertexShader : public RHI::FRHIVertexShader
{
protected:
    ID3D11VertexShader* Resource;
public:
    FD3D11VertexShader(ID3D11VertexShader* InResource, RHI::FRHIShaderDesc InDesc)
        : RHI::FRHIVertexShader(InDesc)
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

struct FD3D11PixelShader : public RHI::FRHIPixelShader
{
protected:
    ID3D11PixelShader* Resource;
public:
    FD3D11PixelShader(ID3D11PixelShader* InResource, RHI::FRHIShaderDesc InDesc)
        : RHI::FRHIPixelShader(InDesc)
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

struct FD3D11ComputeShader : public RHI::FRHIComputeShader
{
protected:
    ID3D11ComputeShader* Resource;
public:
    FD3D11ComputeShader(ID3D11ComputeShader* InResource, RHI::FRHIShaderDesc InDesc)
        : RHI::FRHIComputeShader(InDesc)
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

struct FD3D11GeometryShader : public RHI::FRHIGeometryShader
{
protected:
    ID3D11GeometryShader* Resource;
public:
    FD3D11GeometryShader(ID3D11GeometryShader* InResource, RHI::FRHIShaderDesc InDesc)
        : RHI::FRHIGeometryShader(InDesc)
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

struct FD3D11View : public RHI::FRHIView
{
protected:
    union
    {
        ID3D11ShaderResourceView* SRV;
        ID3D11RenderTargetView* RTV;
        ID3D11DepthStencilView* DSV;
        ID3D11UnorderedAccessView* UAV;
        void* Ptr;
    } Resource;

public:
    virtual void Release() override final
    {
        if (Resource.Ptr)
        {
            if (ViewTarget == RHI::EViewTarget::SRV)
            {
                Resource.SRV->Release();
            }
            else if (ViewTarget == RHI::EViewTarget::RTV)
            {
                Resource.RTV->Release();
            }
            else if (ViewTarget == RHI::EViewTarget::DSV)
            {
                Resource.DSV->Release();
            }
            else if (ViewTarget == RHI::EViewTarget::UAV)
            {
                Resource.UAV->Release();
            }
        }
    }

    void* GetNativeView() const
    {
        return Resource.Ptr;
    }

    void SetNativeView(void* InView)
    {
        Resource.Ptr = InView;
    }
};

struct FD3D11Texture : public RHI::FRHITexture
{
protected:
    ID3D11Texture2D* Resource;

public:
    FD3D11Texture(ID3D11Texture2D* InResource, ID3D11ShaderResourceView* InSRV, ID3D11RenderTargetView* InRTV, ID3D11DepthStencilView* InDSV, ID3D11UnorderedAccessView* InUAV, RHI::FRHITextureDesc InDesc)
        : RHI::FRHITexture(InDesc)
        , Resource(InResource)
    {
        if (InSRV)
        {
            FD3D11View* NewSRV = new FD3D11View();
            NewSRV->ViewTarget = RHI::EViewTarget::SRV;
            NewSRV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
            NewSRV->Format = Desc.Format;
            NewSRV->MostDetailedMip = Desc.MostDetailedMip;
            NewSRV->MipLevels = Desc.MipLevels;
            NewSRV->SetNativeView(InSRV);
            SRV = NewSRV;
        }

        if (InRTV)
        {
            FD3D11View* NewRTV = new FD3D11View();
            NewRTV->ViewTarget = RHI::EViewTarget::RTV;
            NewRTV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
            NewRTV->Format = Desc.Format;
            NewRTV->MostDetailedMip = Desc.MostDetailedMip;
            NewRTV->MipLevels = Desc.MipLevels;
            NewRTV->SetNativeView(InRTV);
            RTV = NewRTV;
        }

        if (InDSV)
        {
            FD3D11View* NewDSV = new FD3D11View();
            NewDSV->ViewTarget = RHI::EViewTarget::DSV;
            NewDSV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
            NewDSV->Format = Desc.Format;
            NewDSV->MostDetailedMip = Desc.MostDetailedMip;
            NewDSV->MipLevels = Desc.MipLevels;
            NewDSV->SetNativeView(InDSV);
            DSV = NewDSV;
        }

        if (InUAV)
        {
            FD3D11View* NewUAV = new FD3D11View();
            NewUAV->ViewTarget = RHI::EViewTarget::UAV;
            NewUAV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
            NewUAV->Format = Desc.Format;
            NewUAV->MostDetailedMip = Desc.MostDetailedMip;
            NewUAV->MipLevels = Desc.MipLevels;
            NewUAV->SetNativeView(InUAV);
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

struct FD3D11SamplerState : public RHI::FRHISamplerState
{
protected:
    ID3D11SamplerState* Resource;
public:
    FD3D11SamplerState(ID3D11SamplerState* InResource, RHI::FRHISamplerStateDesc InDesc)
        : RHI::FRHISamplerState(InDesc)
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

struct FD3D11RasterizerState : public RHI::FRHIRasterizerState
{
protected:
    ID3D11RasterizerState* Resource;
public:
    FD3D11RasterizerState(ID3D11RasterizerState* InResource, RHI::FRHIRasterizerStateDesc InDesc)
        : RHI::FRHIRasterizerState(InDesc)
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

struct FD3D11BlendState : public RHI::FRHIBlendState
{
protected:
    ID3D11BlendState* Resource;
public:
    FD3D11BlendState(ID3D11BlendState* InResource, RHI::FRHIBlendStateDesc InDesc)
        : RHI::FRHIBlendState(InDesc)
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

struct FD3D11DepthStencilState : public RHI::FRHIDepthStencilState
{
public:
    ID3D11DepthStencilState* Resource;
    FD3D11DepthStencilState(ID3D11DepthStencilState* InResource, RHI::FRHIDepthStencilStateDesc InDesc)
        : RHI::FRHIDepthStencilState(InDesc)
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
