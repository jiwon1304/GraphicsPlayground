// ParticleRenderPass.h
#pragma once

#include "IRenderPass.h"
#include "Define.h"

struct FDynamicSpriteEmitterReplayDataBase;
struct FParticleEmitterInstance;
struct FTexture;
class UParticleSystemComponent;

class FParticleRenderPass : public IRenderPass
{
public:
    FParticleRenderPass();
    virtual ~FParticleRenderPass();

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;

    virtual void PrepareRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void ClearRenderArr() override;

private:
    void CreateShader();
    void ReleaseShader();
    void RenderSpriteEmitter(UParticleSystemComponent* Comp, FParticleEmitterInstance* Emitter, const FDynamicSpriteEmitterReplayDataBase& ReplayData);

    void PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport);

private:
    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;
    FVertexInfo InstanceInfoSprite;
    FVertexInfo QuadVertexInfo;
    TArray<UParticleSystemComponent*> ParticleComponents;

    //FDynamicSpriteEmitterReplayDataBase의
    //MaterialInterface가 구현이 안 되어있어 임시 사용
    std::shared_ptr<FTexture> TestTexture;
};
