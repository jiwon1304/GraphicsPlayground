#pragma once
#include "Engine/SkeletalMesh.h"
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
    void RenderAddPrimitiveButton();
    void RenderPhysicsAssetFilter();
    void RenderSkeletonBoneTree();
    void RenderDetailPanel();

    // 각 Bone에 대해 순회
    void RenderTreeRecursive(USkeletalMesh* InSkeletalMesh, UPhysicsAsset* InPhysicsAsset, int32 InBoneIndex, uint8 bShowBones, uint8 bShowBoneIndices, uint8 bShowBodies, uint8 bShowConstraints, uint8 bShowPrimitives);
    
    FString GetCleanBoneName(const FMeshBoneInfo& BoneInfo, int32 BoneIndex, uint8 bShowBoneIndices) const;
    void LoadBoneIcon();
    
private:
    float Width = 0, Height = 0;

    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;

    ID3D11ShaderResourceView* BodySetupIconSRV = nullptr;
    ID3D11ShaderResourceView* BoxIconSRV = nullptr;
    ID3D11ShaderResourceView* SphereIconSRV = nullptr;
    ID3D11ShaderResourceView* SphylIconSRV = nullptr;
};
