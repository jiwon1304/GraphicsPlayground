#include "ParticleModuletypeDataMesh.h"

#include "Engine/FObjLoader.h"
#include "Particles/ParticleHelper.h"
#include "Particles/ParticleEmitterInstances.h"

FParticleEmitterInstance* UParticleModuleTypeDataMesh::CreateInstance(UParticleEmitter* InEmitterParent, UParticleSystemComponent* InComponent)
{
    // !TODO : 기본 메시 세팅

    FParticleEmitterInstance* Instance = new FParticleMeshEmitterInstance();
    return Instance;
}
UParticleModuleTypeDataMesh::UParticleModuleTypeDataMesh()
{
    //Test용 기본 메시
    Mesh=FObjManager::GetStaticMesh(L"Contents/Reference/Reference.obj");
};
