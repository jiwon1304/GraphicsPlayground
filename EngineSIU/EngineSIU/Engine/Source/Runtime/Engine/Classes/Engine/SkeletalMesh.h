#pragma once
#include "SkinnedAsset.h"
#include "Animation/Skeleton.h"
#include "Asset/SkeletalMeshAsset.h" 
class UPhysicsAsset;
class USkeleton;
//struct FSkeletalMeshRenderData;

class USkeletalMesh : public USkinnedAsset
{
    DECLARE_CLASS(USkeletalMesh, USkinnedAsset)

public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override = default;

    void SetRenderData(std::unique_ptr<FSkeletalMeshRenderData> InRenderData);

    const FSkeletalMeshRenderData* GetRenderData() const;

    USkeleton* GetSkeleton() const { return Skeleton; }

    void SetSkeleton(USkeleton* InSkeleton) { Skeleton = InSkeleton; }

    virtual void SerializeAsset(FArchive& Ar) override;

    virtual UPhysicsAsset* GetPhysicsAsset() const
    {
        return PhysicsAsset;
    }

    void SetPhysicsAsset(UPhysicsAsset* InPhysicsAsset)
    {
        PhysicsAsset = InPhysicsAsset;
    }
    const FReferenceSkeleton* GetRefSkeleton()
    {
        if (Skeleton)
        {
            return &Skeleton->GetReferenceSkeleton();
        }
        return nullptr;
    }

    
protected:
    std::unique_ptr<FSkeletalMeshRenderData> RenderData;

    USkeleton* Skeleton;

    UPhysicsAsset* PhysicsAsset = nullptr;
};
