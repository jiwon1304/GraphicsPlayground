#include "ParticleModuleRequired.h"
#include "Particles/ParticleEmitterInstances.h"
#include "UObject/ObjectFactory.h"

void UParticleModuleRequired::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
}

void UParticleModuleRequired::PostInitProperties()
{
    InitializeDefaults();
}

void UParticleModuleRequired::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

}

void UParticleModuleRequired::InitializeDefaults()
{
    if (!Material)
    {
        Material = FObjectFactory::ConstructObject<UMaterial>(nullptr);
        FTextureInfo TexInfo;
        TexInfo.TextureName = TEXT("T_Explosion_SubUV");
        TexInfo.TexturePath = L"Assets/Texture/T_Explosion_SubUV.png"; // 경로는 Wide

        TexInfo.bIsSRGB = true;
        Material->GetMaterialInfo().TextureInfos.Add(TexInfo);
    }
}
