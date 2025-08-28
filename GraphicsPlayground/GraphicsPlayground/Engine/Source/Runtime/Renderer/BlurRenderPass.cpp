#include "BlurRenderPass.h"
#include "Engine/Engine.h"
#include "Engine/World/World.h"
#include "Classes/Camera/PlayerCameraManager.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Renderer/ShaderConstants.h"
#include "RendererHelpers.h"
#include "UnrealClient.h"


FBlurRenderPass::FBlurRenderPass()
{
}

FBlurRenderPass::~FBlurRenderPass()
{
    ReleaseShader();
}

void FBlurRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    Graphics = InGraphics;
    BufferManager = InBufferManager;
    ShaderManager = InShaderManager;
    CreateShader();
    CreateBlendState();
    CreateSampler();
}

void FBlurRenderPass::ReleaseShader()
{
}

void FBlurRenderPass::CreateShader()
{
    ShaderManager->AddVertexShaderAsync(L"BlurEffectVertexShader", L"Shaders/BlurEffectShader.hlsl", "mainVS", nullptr);
    ShaderManager->AddPixelShaderAsync(L"BlurEffectPixelShader", L"Shaders/BlurEffectShader.hlsl", "mainPS", nullptr);
    BufferManager->CreateBufferGeneric<FConstantBufferDOF>("DOFConstantBuffer", nullptr, sizeof(FConstantBufferDOF), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void FBlurRenderPass::UpdateShader()
{
    VertexShader = ShaderManager->GetVertexShaderByKey(L"BlurEffectVertexShader");
    PixelShader = ShaderManager->GetPixelShaderByKey(L"BlurEffectPixelShader");
}

void FBlurRenderPass::CreateBlendState()
{
}

void FBlurRenderPass::CreateSampler()
{
    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    Graphics->Device->CreateSamplerState(&SamplerDesc, &Sampler);
}


void FBlurRenderPass::PrepareRenderState()
{
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);

    Graphics->DeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    Graphics->DeviceContext->IASetInputLayout(nullptr);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);

    Graphics->DeviceContext->PSSetSamplers(0, 1, &Sampler);
}

void FBlurRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    if (!GEngine->ActiveWorld->GetPlayerController() || !GEngine->ActiveWorld->GetPlayerController()->PlayerCameraManager)
    {
        return;
    }
    const FPostProcessSettings& PostProcessSettings = GEngine->ActiveWorld->GetPlayerController()->PlayerCameraManager->PostProcessSettings;
    if (!PostProcessSettings.bDepthOfField)
    {
        return;
    }

    // 해당 뷰포트 크기만큼 새로운 텍스쳐를 생성합니다.
    // rgb값은 저장되고, 알파 채널에는 blurriness 가 저장됩니다.
    // focal distance에서 focal distance + focal region만큼은 0 이 저장되며, 멀어질수록 1이 됩니다.
    // TODO : Focal Distance가 현재는 normalized depth 기준이지만, 
    // 역계산해서 world depth 기준이 되도록 변경하기.

    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    
    // SRV (Scene) 설정
    // Scene의 Texture를 이용합니다.
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, 
        &ViewportResource->GetRenderTarget(EResourceType::ERT_Scene)->SRV);
    // SRV (Scene Depth) 설정
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_SceneDepth), 1,
        &ViewportResource->GetDepthStencil(EResourceType::ERT_Scene)->SRV);


    // Constant buffer 설정
    FConstantBufferDOF DOFParams;
    BufferManager->BindConstantBuffer("DOFConstantBuffer", 0, EShaderStage::Pixel);

    DOFParams.FocalDistance = PostProcessSettings.DepthOfFieldFocalDistance;
    DOFParams.FocalRegion = PostProcessSettings.DepthOfFieldFocalRegion;
    DOFParams.BlurAmount = PostProcessSettings.DepthOfFieldDepthBlurAmount;
    DOFParams.BlurRadius = PostProcessSettings.DepthOfFieldDepthBlurRadius;

    BufferManager->UpdateConstantBuffer("DOFConstantBuffer", DOFParams);

    // RTV 설정
    const EResourceType ResourceType = EResourceType::ERT_PP_Blur;
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, nullptr);

    UpdateShader();
    PrepareRenderState();

    Graphics->DeviceContext->Draw(6, 0);

    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}
 
