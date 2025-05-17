#include "ParticleRenderPass.h"

#include "World/World.h"
#include "EngineLoop.h"

#include "UnrealClient.h"
#include "UObject/UObjectIterator.h"
#include "UObject/Casts.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Editor/PropertyEditor/ShowFlags.h"

#include "D3D11RHI/DXDBufferManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"

#include "ParticleHelper.h"
#include "Particles/ParticleEmitterInstance.h"
#include "Particles/ParticleSystemComponent.h"

FParticleRenderPass::FParticleRenderPass()
    : BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
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

void FParticleRenderPass::PrepareRenderArr()
{
    for (UParticleSystemComponent* Comp : TObjectRange<UParticleSystemComponent>())
    {
        if (Comp && Comp->GetWorld() == GEngine->ActiveWorld)
        {
            ParticleSystemComponents.Add(Comp);
        }
    }
}

void FParticleRenderPass::ClearRenderArr()
{
    ParticleSystemComponents.Empty();
}

void FParticleRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRenderState(Viewport);
    RenderAllParticleSystems(Viewport);
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void FParticleRenderPass::PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    BufferManager->BindConstantBuffer(TEXT("FObjectConstantBuffer"), 12, EShaderStage::Vertex);

    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RT = ViewportResource->GetRenderTarget(EResourceType::ERT_Scene);
    FDepthStencilRHI* DS = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);

    Graphics->DeviceContext->RSSetViewports(1, &ViewportResource->GetD3DViewport());
    Graphics->DeviceContext->OMSetRenderTargets(1, &RT->RTV, DS->DSV);
}

void FParticleRenderPass::RenderAllParticleSystems(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    for (UParticleSystemComponent* Comp : ParticleSystemComponents)
    {
        if (!Comp) continue;

        const TArray<FDynamicEmitterDataBase*>& RenderDataArr = Comp->GetEmitterRenderData();

        for (FDynamicEmitterDataBase* RenderData : RenderDataArr)
        {
            if (!RenderData || !RenderData->bValid) continue;

            const EDynamicEmitterType EmitterType = RenderData->GetSource().eEmitterType;

            const FMatrix WorldMatrix = Comp->GetWorldMatrix();
            const FVector4 UUIDColor = Comp->EncodeUUID() / 255.0f;
            UpdateObjectConstant(WorldMatrix, UUIDColor, false);

            // Shader 설정 분기 (Define 기반)
            D3D_SHADER_MACRO DefineMesh[] = { {"PARTICLE_MESH", "1"}, {nullptr, nullptr} };
            D3D_SHADER_MACRO DefineSprite[] = { {"PARTICLE_SPRITE", "1"}, {nullptr, nullptr} };

            switch (EmitterType)
            {
            case DET_Sprite:
                ShaderManager->AddVertexShaderAndInputLayout(L"ParticleSpriteVS", L"Shaders/ParticleShader.hlsl", "mainVS", nullptr, 0, DefineSprite);
                ShaderManager->AddPixelShader(L"ParticleSpritePS", L"Shaders/ParticleShader.hlsl", "mainPS", DefineSprite);
                Graphics->DeviceContext->VSSetShader(ShaderManager->GetVertexShaderByKey(L"ParticleSpriteVS"), nullptr, 0);
                Graphics->DeviceContext->IASetInputLayout(ShaderManager->GetInputLayoutByKey(L"ParticleSpriteVS"));
                Graphics->DeviceContext->PSSetShader(ShaderManager->GetPixelShaderByKey(L"ParticleSpritePS"), nullptr, 0);
                RenderSpriteEmitter(RenderData);
                break;
            case DET_Mesh:
                ShaderManager->AddVertexShaderAndInputLayout(L"ParticleMeshVS", L"Shaders/ParticleShader.hlsl", "mainVS", nullptr, 0, DefineMesh);
                ShaderManager->AddPixelShader(L"ParticleMeshPS", L"Shaders/ParticleShader.hlsl", "mainPS", DefineMesh);
                Graphics->DeviceContext->VSSetShader(ShaderManager->GetVertexShaderByKey(L"ParticleMeshVS"), nullptr, 0);
                Graphics->DeviceContext->IASetInputLayout(ShaderManager->GetInputLayoutByKey(L"ParticleMeshVS"));
                Graphics->DeviceContext->PSSetShader(ShaderManager->GetPixelShaderByKey(L"ParticleMeshPS"), nullptr, 0);
                RenderMeshEmitter(RenderData);
                break;
            default:
                break;
            }
        }
    }
}

void FParticleRenderPass::RenderSpriteEmitter(FDynamicEmitterDataBase* RenderData)
{
    FDynamicSpriteEmitterDataBase* SpriteDataBase = static_cast<FDynamicSpriteEmitterDataBase*>(RenderData);
    if (!SpriteDataBase) return;

    const FDynamicSpriteEmitterReplayDataBase& Source = static_cast<const FDynamicSpriteEmitterReplayDataBase&>(SpriteDataBase->GetSource());
    const int32 Count = Source.ActiveParticleCount;
    if (Count == 0 || !Source.DataContainer.ParticleData) return;

    const int32 Stride = Source.ParticleStride;
    TArray<FParticleSpriteVertex> Vertices;
    Vertices.SetNum(Count);

    for (int32 i = 0; i < Count; ++i)
    {
        const void* Ptr = Source.DataContainer.ParticleData + i * Stride;
        Vertices[i] = *reinterpret_cast<const FParticleSpriteVertex*>(Ptr);
    }

    FVertexInfo VertexInfo;
    if (BufferManager->CreateDynamicVertexBuffer(TEXT("SpriteParticle"), Vertices, VertexInfo) != S_OK)
        return;

    BufferManager->UpdateDynamicVertexBuffer(TEXT("SpriteParticle"), Vertices);

    UINT Offset = 0;
    UINT VBStride = sizeof(FParticleSpriteVertex);
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &VBStride, &Offset);
    Graphics->DeviceContext->Draw(Count * 6, 0);
}

void FParticleRenderPass::RenderMeshEmitter(FDynamicEmitterDataBase* RenderData)
{
    FDynamicMeshEmitterData* MeshData = static_cast<FDynamicMeshEmitterData*>(RenderData);
    if (!MeshData) return;

    const FDynamicMeshEmitterReplayData& Source = static_cast<const FDynamicMeshEmitterReplayData&>(MeshData->GetSource());
    const int32 Count = Source.ActiveParticleCount;
    if (Count == 0 || !Source.DataContainer.ParticleData) return;

    const int32 Stride = Source.ParticleStride;
    TArray<FMeshParticleInstanceVertex> Vertices;
    Vertices.SetNum(Count);

    for (int32 i = 0; i < Count; ++i)
    {
        const void* Ptr = Source.DataContainer.ParticleData + i * Stride;
        Vertices[i] = *reinterpret_cast<const FMeshParticleInstanceVertex*>(Ptr);
    }

    FVertexInfo VertexInfo;
    if (BufferManager->CreateDynamicVertexBuffer(TEXT("MeshParticle"), Vertices, VertexInfo) != S_OK)
        return;

    BufferManager->UpdateDynamicVertexBuffer(TEXT("MeshParticle"), Vertices);

    UINT Offset = 0;
    UINT VBStride = sizeof(FMeshParticleInstanceVertex);
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &VBStride, &Offset);
    Graphics->DeviceContext->Draw(Count * 3, 0);
}

void FParticleRenderPass::UpdateObjectConstant(const FMatrix& WorldMatrix, const FVector4& UUIDColor, bool bIsSelected) const
{
    FObjectConstantBuffer ObjectData = {};
    ObjectData.WorldMatrix = WorldMatrix;
    ObjectData.InverseTransposedWorld = FMatrix::Transpose(FMatrix::Inverse(WorldMatrix));
    ObjectData.UUIDColor = UUIDColor;
    ObjectData.bIsSelected = bIsSelected;

    BufferManager->UpdateConstantBuffer(TEXT("FObjectConstantBuffer"), ObjectData);
}

void FParticleRenderPass::CreateShader()
{
    // 셰이더 로딩은 ChangeViewMode에서 Define 포함으로 자동 처리
}

void FParticleRenderPass::ReleaseShader()
{
}
