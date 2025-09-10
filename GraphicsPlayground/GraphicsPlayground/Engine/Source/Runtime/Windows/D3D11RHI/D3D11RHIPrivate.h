#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <d3dcompiler.h>

#include "RHI/DynamicRHI.h"
#include "D3D11Resources.h"

class FD3D11Viewport;

class FD3D11DynamicRHI : public ID3D11DynamicRHI
{
    public:
    virtual ~FD3D11DynamicRHI() override = default;
    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void RHITick(float DeltaTime) override;
    virtual void RHIExecuteCommandList(FRHICommandList* CmdList) override;
    // -------------------------------------------------------------
    // Resource Creation
    // -------------------------------------------------------------
    virtual FRHIBufferRef CreateBuffer(const FRHIBufferDesc& Desc, const void* InitialData) override final;
    virtual FRHIVertexDeclarationRef CreateInputLayout(const TArray<FVertexElement>& Attributes, const FRHIVertexShaderRef& VertexShader) override final;
    virtual FRHIVertexShaderRef CreateVertexShader(const FRHIShaderDesc& Desc) override final;
    virtual FRHIPixelShaderRef CreatePixelShader(const FRHIShaderDesc& Desc) override final;
    virtual FRHIComputeShaderRef CreateComputeShader(const FRHIShaderDesc& Desc) override final;
    virtual FRHIGeometryShaderRef CreateGeometryShader(const FRHIShaderDesc& Desc) override final;
    virtual FRHITextureRef CreateTexture(const FRHITextureDesc& Desc, const void* InitialData) override final;
    virtual FRHISamplerStateRef CreateSamplerState(const FRHISamplerStateDesc& Desc) override final;
    virtual FRHIRasterizerStateRef CreateRasterizerState(const FRHIRasterizerStateDesc& Desc) override final;
    virtual FRHIBlendStateRef CreateBlendState(const FRHIBlendStateDesc& Desc) override final;
    virtual FRHIDepthStencilStateRef CreateDepthStencilState(const FRHIDepthStencilStateDesc& Desc) override final;
    virtual FRHIUniformBufferRef CreateUniformBuffer(const FRHIUniformBufferLayout* Layout, EUniformBufferUsage Usage) override final;
    virtual FRHIViewportRef CreateViewport(const FRHIViewportDesc& Desc) override final;
protected:
    TSharedPtr<IDXGIFactory> DXGIFactory;
    TSharedPtr<ID3D11Device> Direct3DDevice;
    TSharedPtr<ID3D11DeviceContext> Direct3DDeviceIMContext;

    TArray<FD3D11Viewport*> Viewports;
    TSharedPtr<FD3D11Viewport> DrawingViewport;

    TSharedPtr<IDXGIAdapter> Adapter;
};
