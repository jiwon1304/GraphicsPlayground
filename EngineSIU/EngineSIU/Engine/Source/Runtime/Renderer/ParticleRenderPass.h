// File: Engine/Source/Runtime/Renderer/RenderPass/ParticleRenderPass.h

#pragma once

#include "IRenderPass.h"
#include "EngineBaseTypes.h"
#include "Container/Set.h"

#include "Define.h"

class FDynamicEmitterDataBase;
class UParticleSystemComponent;
class UParticleSubUVComponent;
class FDXDBufferManager;
class FGraphicsDevice;
class FDXDShaderManager;
class FEditorViewportClient;
class UWorld;
class UMaterial;

/**
 * 파티클 렌더링 전용 패스. 시스템 파티클과 SubUV 파티클 모두 처리.
 */
class FParticleRenderPass : public IRenderPass
{
public:
    FParticleRenderPass();
    virtual ~FParticleRenderPass();

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;

    virtual void PrepareRenderArr() override;
    virtual void ClearRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

protected:
    void PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport);

    /** 실제 렌더링 호출 */
    void RenderAllParticleSystems(const std::shared_ptr<FEditorViewportClient>& Viewport);

    /** Sprite 기반 파티클 렌더 */
    void RenderSpriteEmitter(FDynamicEmitterDataBase* RenderData);

    /** Mesh 기반 파티클 렌더 */
    void RenderMeshEmitter(FDynamicEmitterDataBase* RenderData);

    /** 공통 상수 버퍼 업데이트 */
    void UpdateObjectConstant(const FMatrix& WorldMatrix, const FVector4& UUIDColor, bool bIsSelected) const;

    /** 뷰모드에 따른 쉐이더 바인딩 */
    void ChangeViewMode(EViewModeIndex ViewMode);

    /** 쉐이더 생성 / 해제 */
    void CreateShader();
    void ReleaseShader();

protected:
    /** 현재 월드에 존재하는 ParticleSystemComponent 목록 */
    TArray<UParticleSystemComponent*> ParticleSystemComponents;

    FDXDBufferManager* BufferManager = nullptr;
    FGraphicsDevice* Graphics = nullptr;
    FDXDShaderManager* ShaderManager = nullptr;
};
