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
    D3D11_INPUT_ELEMENT_DESC ParticleInputLayout[] = {
        { "POSITION",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       0, DXGI_FORMAT_R32_FLOAT,           0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       1, DXGI_FORMAT_R32G32B32_FLOAT,     0, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       2, DXGI_FORMAT_R32_FLOAT,           0, 28,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       3, DXGI_FORMAT_R32G32_FLOAT,        0, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       4, DXGI_FORMAT_R32_FLOAT,           0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       5, DXGI_FORMAT_R32_FLOAT,           0, 44,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",          0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",       6, DXGI_FORMAT_R32G32_FLOAT,        0, 64,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    HRESULT hr = ShaderManager->AddVertexShaderAndInputLayout(
        L"ParticleShader",
        L"Shaders/ParticleShader.hlsl",
        "mainVS",
        ParticleInputLayout,
        ARRAYSIZE(ParticleInputLayout)
    );

    if (FAILED(hr))
    {
        return;
    }

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
        if (!Comp || Comp->GetEmitterRenderData().IsEmpty())
            continue;

        const FString Key = Comp->GetName();

        TArray<FParticleSpriteVertex> AllVertices;
        for (FDynamicEmitterDataBase* BaseEmitterData : Comp->GetEmitterRenderData())
        {
            if (!BaseEmitterData || !BaseEmitterData->bValid)
                continue;

            const FDynamicEmitterReplayDataBase& ReplayData = BaseEmitterData->GetSource();
            if (ReplayData.eEmitterType != EDynamicEmitterType::DET_Sprite)
                continue;

            const FDynamicSpriteEmitterReplayDataBase* SpriteData = static_cast<const FDynamicSpriteEmitterReplayDataBase*>(&ReplayData);

            const uint8* ParticleData = SpriteData->DataContainer.ParticleData;
            const uint16* ParticleIndices = SpriteData->DataContainer.ParticleIndices;
            const int32 Stride = SpriteData->ParticleStride;
            const int32 ActiveCount = SpriteData->ActiveParticleCount;

            static const FVector2D UVs[6] = {
                {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f},
                {-0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}
            };

            for (int32 i = 0; i < ActiveCount; ++i)
            {
                const int32 ParticleIndex = ParticleIndices[i];
                const FBaseParticle* P = reinterpret_cast<const FBaseParticle*>(ParticleData + ParticleIndex * Stride);

                for (int j = 0; j < 6; ++j)
                {
                    FParticleSpriteVertex V;
                    V.Position = P->Location;
                    V.OldPosition = P->OldLocation;
                    V.RelativeTime = P->RelativeTime;
                    V.ParticleId = static_cast<float>(i);
                    V.Size = FVector2D(P->Size.X, P->Size.Y);
                    V.Rotation = P->Rotation;
                    V.SubImageIndex = 0.0f;
                    V.Color = P->Color;
                    V.UV = UVs[j];
                    AllVertices.Add(V);
                }
            }
        }

        if (AllVertices.IsEmpty())
            continue;

        FVertexInfo VertexInfo;
        if (!BufferManager->GetVertexBuffer(Key).VertexBuffer)
        {
            BufferManager->CreateDynamicVertexBuffer(Key, AllVertices, VertexInfo);
        }
        else
        {
            BufferManager->UpdateDynamicVertexBuffer(Key, AllVertices);
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
