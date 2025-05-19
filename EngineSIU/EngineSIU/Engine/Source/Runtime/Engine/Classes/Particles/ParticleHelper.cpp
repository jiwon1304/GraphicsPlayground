#include "ParticleHelper.h"
#include "Particles/ParticleModules/ParticleModuleRequired.h"

FDynamicEmitterDataBase::FDynamicEmitterDataBase(const UParticleModuleRequired* RequiredModule)
    :EmitterIndex(INDEX_NONE)
{

}

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    assert(InParticleDataNumBytes > 0 && ParticleIndicesNumShorts >= 0 && InParticleDataNumBytes && sizeof(uint16) == 0);

    ParticleDataNumBytes = InParticleDataNumBytes;
    ParticleIndicesNumShorts = InParticleIndicesNumShorts;

    MemBlockSize = ParticleDataNumBytes + ParticleIndicesNumShorts * sizeof(uint16);

    ParticleData = (uint8*)malloc(MemBlockSize);
    ParticleIndices = (uint16*)(ParticleData + ParticleDataNumBytes);
}

void FParticleDataContainer::Free()
{
    if (ParticleData)
    {
        assert(MemBlockSize > 0);
        free(ParticleData);
    }
    MemBlockSize = 0;
    ParticleDataNumBytes = 0;
    ParticleIndicesNumShorts = 0;
    ParticleData = nullptr;
    ParticleIndices = nullptr;
}
