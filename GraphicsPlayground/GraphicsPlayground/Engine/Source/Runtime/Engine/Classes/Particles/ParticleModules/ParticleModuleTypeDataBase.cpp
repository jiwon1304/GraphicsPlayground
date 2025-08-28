#include "ParticleModuleTypeDataBase.h"

FParticleEmitterInstance* UParticleModuleTypeDataBase::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    return nullptr;
}

void UParticleModuleTypeDataBase::PostInitProperties()
{
    bEnabled = true;
}
