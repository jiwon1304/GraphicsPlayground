// ParticleRenderPass.cpp
#include "ParticleRenderPass.h"

#include "EngineLoop.h"
#include "ParticleHelper.h"
#include "Engine/Engine.h"
#include "UnrealEd/EditorViewportClient.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealClient.h"
#include "RendererHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/UObjectIterator.h"

FParticleRenderPass::FParticleRenderPass()
    : BufferManager(nullptr), Graphics(nullptr), ShaderManager(nullptr)
{
}

FParticleRenderPass::~FParticleRenderPass()
{
    ReleaseShader();
}

void FParticleRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManager;

    CreateShader();
}

void FParticleRenderPass::CreateShader()
{
    ShaderManager->AddVertexShader(L"ParticleShader", L"Shaders/ParticleShader.hlsl", "mainVS");
    ShaderManager->AddPixelShader(L"ParticleShader", L"Shaders/ParticleShader.hlsl", "mainPS");
}

void FParticleRenderPass::ReleaseShader()
{
}

void FParticleRenderPass::PrepareRenderArr()
{
    ParticleComponents.Empty();

    for (auto Comp : TObjectRange<UParticleSystemComponent>())
    {
        if (Comp->GetWorld() == GEngine->ActiveWorld && Comp->GetEmitterType() == EDynamicEmitterType::DET_Sprite)
        {
            ParticleComponents.Add(Comp);
        }
    }
}

void FParticleRenderPass::PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11VertexShader* VS = ShaderManager->GetVertexShaderByKey(L"ParticleShader");
    ID3D11InputLayout* IL = ShaderManager->GetInputLayoutByKey(L"ParticleShader");
    ID3D11PixelShader* PS = ShaderManager->GetPixelShaderByKey(L"ParticleShader");

    Graphics->DeviceContext->VSSetShader(VS, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(IL);
    Graphics->DeviceContext->PSSetShader(PS, nullptr, 0);

    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RT = ViewportResource->GetRenderTarget(EResourceType::ERT_Scene);
    FDepthStencilRHI* DS = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);
    Graphics->DeviceContext->RSSetViewports(1, &ViewportResource->GetD3DViewport());
    Graphics->DeviceContext->OMSetRenderTargets(1, &RT->RTV, DS->DSV);
}

void FParticleRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRenderState(Viewport);

    for (UParticleSystemComponent* Comp : ParticleComponents)
    {
        if (!Comp) continue;

        const FString Key = Comp->GetName();
        const TArray<FParticleSpriteVertex>& Vertices = Comp->GetVertexData();

        if (Vertices.IsEmpty())
            continue;

        FVertexInfo VertexInfo;
        if (!BufferManager->GetVertexBuffer(Key).VertexBuffer)
        {
            BufferManager->CreateDynamicVertexBuffer(Key, Vertices, VertexInfo);
        }
        else
        {
            BufferManager->UpdateDynamicVertexBuffer(Key, Vertices);
            VertexInfo = BufferManager->GetVertexBuffer(Key);
        }

        FObjectConstantBuffer ObjectData;
        FMatrix WorldMatrix = Comp->GetWorldMatrix();
        ObjectData.WorldMatrix = WorldMatrix;
        ObjectData.InverseTransposedWorld = FMatrix::Transpose(FMatrix::Inverse(WorldMatrix));
        ObjectData.UUIDColor = Comp->EncodeUUID() / 255.0f;
        ObjectData.bIsSelected = false;
        BufferManager->UpdateConstantBuffer(TEXT("FObjectConstantBuffer"), ObjectData);

        UINT Stride = sizeof(FParticleSpriteVertex);
        UINT Offset = 0;
        Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &Stride, &Offset);
        Graphics->DeviceContext->Draw(VertexInfo.NumVertices, 0);
    }

    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void FParticleRenderPass::ClearRenderArr()
{
    ParticleComponents.Empty();
}
