#pragma once
#include "OverlayShapeRenderPass.h"

class UStaticMeshComponent;
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
    void RenderStaticComp(UStaticMeshComponent* StaticComp, bool bPreviewWorld);
    void RenderSkelComp(USkeletalMeshComponent* SkelComp, bool bPreviewWorld);
};
