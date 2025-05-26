#pragma once
#include "GameFramework/Actor.h"   // TODO - Actor를 상속해야 작동함. 상위 클래스에서 문제 발생함. uint8에 접근 못하는듯
#include "UnrealEd/EditorPanel.h"

struct FReferenceSkeleton;

class FPhysicsAssetEditorPanel : public UEditorPanel
{
public:
    FPhysicsAssetEditorPanel();
    virtual ~FPhysicsAssetEditorPanel() override = default;
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    void RenderAddShapeButton();
    void RenderPhysicsAssetFilter();
    void RenderSkeletonBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex);
    void LoadBoneIcon();
    
private:
    float Width = 0, Height = 0;

    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;
};
