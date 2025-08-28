#pragma once
#include "ParticleModuleTypeDataBase.h"

class UStaticMesh;


// 이미터에 끼워줄 메시타입 모듈
class UParticleModuleTypeDataMesh : public UParticleModuleTypeDataBase
{
    DECLARE_CLASS(UParticleModuleTypeDataMesh, UParticleModuleTypeDataBase)

public:
    UParticleModuleTypeDataMesh();
    virtual ~UParticleModuleTypeDataMesh() override = default;

    virtual FParticleEmitterInstance* CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent) override;

    virtual FName GetModuleName() const override
    {
        return FName(TEXT("MeshTypeData"));
    }
    UPROPERTY(
        EditAnywhere,
        UStaticMesh*, Mesh, = nullptr;
    )
};
