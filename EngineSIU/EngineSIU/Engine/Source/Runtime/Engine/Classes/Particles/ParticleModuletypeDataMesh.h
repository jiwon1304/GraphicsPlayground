#pragma once
#include "ParticleModuleTypeDataBase.h"

class UStaticMesh;

class UParticleModuleTypeDataMesh : public UParticleModuleTypeDataBase
{
    DECLARE_CLASS(UParticleModuleTypeDataMesh, UParticleModuleTypeDataBase)
public:
    UParticleModuleTypeDataMesh() = default;
    virtual ~UParticleModuleTypeDataMesh() override = default;

    UStaticMesh* Mesh = nullptr;
};
