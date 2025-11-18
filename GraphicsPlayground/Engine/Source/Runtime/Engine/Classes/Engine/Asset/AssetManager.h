#pragma once
#include "Engine/Classes/Engine/StaticMesh.h"
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Core/Delegates/DelegateCombination.h"


#include "AssetInfo.h"

class UPhysicsAsset;
class UAnimationAsset;
class USkeleton;
class USkeletalMesh;
class UParticleSystem;

DECLARE_DELEGATE_OneParam(FOnAssetAdded, const FAssetInfo& /*InAssetInfo*/);

using FAssetKey = FString;

using FAssetRegistry = TMap<FAssetKey, FAssetInfo>;

struct FAssetRegistry
{
    TMap<FName, FAssetInfo> PathNameToAssetInfo;
};

struct FAssetLoadResult
{
    TArray<USkeleton*> Skeletons;
    TArray<USkeletalMesh*> SkeletalMeshes;
    TArray<UStaticMesh*> StaticMeshes;
    TArray<UMaterial*> Materials;
    TArray<UAnimationAsset*> Animations;
};

struct FSIUAssetLoadResult
{
    TArray<UPhysicsAsset*> PhysicsAssets;
};

class UAssetManager : public UObject
{
    DECLARE_CLASS(UAssetManager, UObject)

private:
    std::unique_ptr<FAssetRegistry> AssetRegistry;

public:
    FOnAssetAdded OnAssetAdded;

    UAssetManager() = default;
    virtual ~UAssetManager() override = default;

    static bool IsInitialized();

    /** UAssetManager를 가져옵니다. */
    static UAssetManager& Get();

    /** UAssetManager가 존재하면 가져오고, 없으면 nullptr를 반환합니다. */
    static UAssetManager* GetIfInitialized();
    
    void InitAssetManager();
    
    const TMap<FName, FAssetInfo>& GetAssetRegistry();
    // TODO - Do not use Directly (Deprecate)
    const TMap<FName, UParticleSystem*>& GetParticleSystemMap() { return ParticleSystemMap; }

    USkeletalMesh* GetSkeletalMesh(const FName& Name);
    UStaticMesh* GetStaticMesh(const FName& Name);
    USkeleton* GetSkeleton(const FName& Name);
    UMaterial* GetMaterial(const FName& Name);
    UAnimationAsset* GetAnimation(const FName& Name);
    UParticleSystem* GetParticleSystem(const FName& Name);
    UPhysicsAsset* GetPhysicsAsset(const FName& Name);

    void AddAssetInfo(const FAssetInfo& Info);
    void AddAssetInfo(UPhysicsAsset* PhysicsAsset) const;

    void RemoveAssetInfo(const UPhysicsAsset* PhysicsAsset) const;

protected:
    void AddSkeleton(const FName& Key, USkeleton* Skeleton);
    void AddSkeletalMesh(const FName& Key, USkeletalMesh* SkeletalMesh);
    void AddMaterial(const FName& Key, UMaterial* Material);
    void AddStaticMesh(const FName& Key, UStaticMesh* StaticMesh);
    void AddAnimation(const FName& Key, UAnimationAsset* Animation);
    void AddParticleSystem(const FName& Key, UParticleSystem* ParticleSystem);
    void AddPhysicsAsset(const FName& Key, UPhysicsAsset* InPhysicsAsset);

    void RemoveParticleSystem(const FName& Key);
    void RemovePhysicsAsset(const FName& Key);

    
    bool SavePhysicsAssetBinary(UPhysicsAsset* PhysicsAsset);
    
private:
    double FbxLoadTime = 0.0;
    double BinaryLoadTime = 0.0;
    double SiuLoadTime = 0.0;
    
    void LoadContentFiles();
    void LoadLazyContentFiles();

    void HandleFBX(const FAssetInfo& AssetInfo);
    void HandleSIU(const FAssetInfo& AssetInfo);
    
    bool LoadPhysicsAssetBinary(const FAssetInfo& AssetInfo);

    void AddToAssetMap(const FAssetLoadResult& Result, const FString& FileName, const FAssetInfo& BaseAssetInfo);

    inline static TMap<FName, USkeleton*> SkeletonMap;
    inline static TMap<FName, USkeletalMesh*> SkeletalMeshMap;
    inline static TMap<FName, UStaticMesh*> StaticMeshMap;
    inline static TMap<FName, UMaterial*> MaterialMap;
    inline static TMap<FName, UAnimationAsset*> AnimationMap;
    inline static TMap<FName, UParticleSystem*> ParticleSystemMap;
    inline static TMap<FName, UPhysicsAsset*> PhysicsAssetMap;

    bool LoadFbxBinary(const FString& FilePath, FAssetLoadResult& Result, const FString& BaseName, const FString& FolderPath);

    bool SaveFbxBinary(const FString& FilePath, FAssetLoadResult& Result, const FString& BaseName, const FString& FolderPath);

    static constexpr uint32 Version = 1;

    bool SerializeVersion(FArchive& Ar);

    bool SerializeAssetLoadResult(FArchive& Ar, FAssetLoadResult& Result, const FString& BaseName, const FString& FolderPath);
    bool SerializeSIUAssetLoadResult(FArchive& Ar, FSIUAssetLoadResult& Result, const FString& BaseName, const FString& FolderPath);
};
