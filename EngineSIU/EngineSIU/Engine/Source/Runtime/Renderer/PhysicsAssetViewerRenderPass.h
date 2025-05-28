#pragma once
#include "OverlayShapeRenderPass.h"

class USkeletalMeshComponent;

class FPhysicsAssetViewerRenderPass : public FOverlayShapeRenderPass
{
    using Super = FOverlayShapeRenderPass;
public:
    FPhysicsAssetViewerRenderPass();
    virtual ~FPhysicsAssetViewerRenderPass() override;
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    virtual void PrepareRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void ClearRenderArr() override;

private:
    void RenderSkelComp(USkeletalMeshComponent* SkelComp);

    // 디버그용
public:
    inline static FVector Axis = FVector(1, 0, 0);
    inline static float Rad = HALF_PI;
    inline static bool Swap = true;
};
