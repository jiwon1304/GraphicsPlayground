// #pragma once

// #include "RHI/RHIResources.h"

// struct FD3D11Buffer : public FRHIBuffer
// {
// protected:
//     ID3D11Buffer* Resource;

// public:
//     FD3D11Buffer(ID3D11Buffer* InResource, FRHIBufferDesc InDesc)
//         : FRHIBuffer(InDesc)
//         , Resource(InResource)
//     {

//     }

//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11Layout : public FRHIVertexDeclaration
// {
// protected:
//     ID3D11InputLayout* Resource;

// public:
//     FD3D11Layout(ID3D11InputLayout* InResource)
//         : Resource(InResource)
//     {

//     }

//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11VertexShader : public FRHIVertexShader
// {
// protected:
//     ID3D11VertexShader* Resource;
// public:
//     FD3D11VertexShader(ID3D11VertexShader* InResource, FRHIShaderDesc InDesc)
//         : FRHIVertexShader(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11PixelShader : public FRHIPixelShader
// {
// protected:
//     ID3D11PixelShader* Resource;
// public:
//     FD3D11PixelShader(ID3D11PixelShader* InResource, FRHIShaderDesc InDesc)
//         : FRHIPixelShader(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11ComputeShader : public FRHIComputeShader
// {
// protected:
//     ID3D11ComputeShader* Resource;
// public:
//     FD3D11ComputeShader(ID3D11ComputeShader* InResource, FRHIShaderDesc InDesc)
//         : FRHIComputeShader(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11GeometryShader : public FRHIGeometryShader
// {
// protected:
//     ID3D11GeometryShader* Resource;
// public:
//     FD3D11GeometryShader(ID3D11GeometryShader* InResource, FRHIShaderDesc InDesc)
//         : FRHIGeometryShader(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11View : public FRHIView
// {
// protected:
//     union
//     {
//         ID3D11ShaderResourceView* SRV;
//         ID3D11RenderTargetView* RTV;
//         ID3D11DepthStencilView* DSV;
//         ID3D11UnorderedAccessView* UAV;
//         void* Ptr;
//     } Resource;

// public:
//     virtual void Release() override final
//     {
//         if (Resource.Ptr)
//         {
//             if (ViewTarget == EViewTarget::SRV)
//             {
//                 Resource.SRV->Release();
//             }
//             else if (ViewTarget == EViewTarget::RTV)
//             {
//                 Resource.RTV->Release();
//             }
//             else if (ViewTarget == EViewTarget::DSV)
//             {
//                 Resource.DSV->Release();
//             }
//             else if (ViewTarget == EViewTarget::UAV)
//             {
//                 Resource.UAV->Release();
//             }
//         }
//     }

//     void* GetNativeView() const
//     {
//         return Resource.Ptr;
//     }

//     void SetNativeView(void* InView)
//     {
//         Resource.Ptr = InView;
//     }
// };

// struct FD3D11Texture : public FRHITexture
// {
// protected:
//     ID3D11Texture2D* Resource;

// public:
//     FD3D11Texture(ID3D11Texture2D* InResource, ID3D11ShaderResourceView* InSRV, ID3D11RenderTargetView* InRTV, ID3D11DepthStencilView* InDSV, ID3D11UnorderedAccessView* InUAV, FRHITextureDesc InDesc)
//         : FRHITexture(InDesc)
//         , Resource(InResource)
//     {
//         if (InSRV)
//         {
//             FD3D11View* NewSRV = new FD3D11View();
//             NewSRV->ViewTarget = EViewTarget::SRV;
//             NewSRV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
//             NewSRV->Format = Desc.Format;
//             NewSRV->MostDetailedMip = Desc.MostDetailedMip;
//             NewSRV->MipLevels = Desc.MipLevels;
//             NewSRV->SetNativeView(InSRV);
//             SRV = NewSRV;
//         }

//         if (InRTV)
//         {
//             FD3D11View* NewRTV = new FD3D11View();
//             NewRTV->ViewTarget = EViewTarget::RTV;
//             NewRTV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
//             NewRTV->Format = Desc.Format;
//             NewRTV->MostDetailedMip = Desc.MostDetailedMip;
//             NewRTV->MipLevels = Desc.MipLevels;
//             NewRTV->SetNativeView(InRTV);
//             RTV = NewRTV;
//         }

//         if (InDSV)
//         {
//             FD3D11View* NewDSV = new FD3D11View();
//             NewDSV->ViewTarget = EViewTarget::DSV;
//             NewDSV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
//             NewDSV->Format = Desc.Format;
//             NewDSV->MostDetailedMip = Desc.MostDetailedMip;
//             NewDSV->MipLevels = Desc.MipLevels;
//             NewDSV->SetNativeView(InDSV);
//             DSV = NewDSV;
//         }

//         if (InUAV)
//         {
//             FD3D11View* NewUAV = new FD3D11View();
//             NewUAV->ViewTarget = EViewTarget::UAV;
//             NewUAV->ViewType = static_cast<RHI::EViewType>(Desc.Type);
//             NewUAV->Format = Desc.Format;
//             NewUAV->MostDetailedMip = Desc.MostDetailedMip;
//             NewUAV->MipLevels = Desc.MipLevels;
//             NewUAV->SetNativeView(InUAV);
//             UAV = NewUAV;
//         }
//     }

//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//         if (SRV)
//         {
//             delete SRV;
//             SRV = nullptr;
//         }
//         if (RTV)
//         {
//             delete RTV;
//             RTV = nullptr;
//         }
//         if (DSV)
//         {
//             delete DSV;
//             DSV = nullptr;
//         }
//         if (UAV)
//         {
//             delete UAV;
//             UAV = nullptr;
//         }
//     }
// };

// struct FD3D11SamplerState : public FRHISamplerState
// {
// protected:
//     ID3D11SamplerState* Resource;
// public:
//     FD3D11SamplerState(ID3D11SamplerState* InResource, FRHISamplerStateDesc InDesc)
//         : FRHISamplerState(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11RasterizerState : public FRHIRasterizerState
// {
// protected:
//     ID3D11RasterizerState* Resource;
// public:
//     FD3D11RasterizerState(ID3D11RasterizerState* InResource, FRHIRasterizerStateDesc InDesc)
//         : FRHIRasterizerState(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11BlendState : public FRHIBlendState
// {
// protected:
//     ID3D11BlendState* Resource;
// public:
//     FD3D11BlendState(ID3D11BlendState* InResource, FRHIBlendStateDesc InDesc)
//         : FRHIBlendState(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };

// struct FD3D11DepthStencilState : public FRHIDepthStencilState
// {
// public:
//     ID3D11DepthStencilState* Resource;
//     FD3D11DepthStencilState(ID3D11DepthStencilState* InResource, FRHIDepthStencilStateDesc InDesc)
//         : FRHIDepthStencilState(InDesc)
//         , Resource(InResource)
//     {
//     }
//     virtual void Release() override final
//     {
//         if (Resource)
//         {
//             Resource->Release();
//             Resource = nullptr;
//         }
//     }
// };
