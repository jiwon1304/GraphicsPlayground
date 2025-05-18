#include "ParticleModuletypeDataMesh.h"

FParticleEmitterInstance* UParticleModuleTypeDataMesh::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    // !TODO : 기본 메시 세팅

    FParticleEmitterInstance* Instance = new FParticleMeshEmitterInstance();
    return nullptr;
}
