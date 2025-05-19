#include "ParticleHelper.h"

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    //check(InParticleDataNumBytes > 0 && ParticleIndicesNumShorts >= 0 && InParticleDataNumBytes % sizeof(uint16) == 0); // we assume that the particle storage has reasonable alignment below
    ParticleDataNumBytes = InParticleDataNumBytes;
    ParticleIndicesNumShorts = InParticleIndicesNumShorts;

    MemBlockSize = ParticleDataNumBytes + ParticleIndicesNumShorts * sizeof(uint16);

    //ParticleData = (uint8*)FastParticleSmallBlockAlloc(MemBlockSize);
    ParticleIndices = (uint16*)(ParticleData + ParticleDataNumBytes);
}

void FParticleDataContainer::Free()
{
    if (ParticleData)
    {
        //check(MemBlockSize > 0);
        //FastParticleSmallBlockFree(ParticleData, MemBlockSize);
    }
    MemBlockSize = 0;
    ParticleDataNumBytes = 0;
    ParticleIndicesNumShorts = 0;
    ParticleData = nullptr;
    ParticleIndices = nullptr;
}

void FDynamicSpriteEmitterReplayDataBase::Serialize(FArchive& Ar)
{
    FDynamicEmitterReplayDataBase::Serialize(Ar);
}
void* FDynamicEmitterDataBase::operator new(size_t Size)
{
    return ::operator new(Size); // 기본 전역 연산자 사용
}

void FDynamicEmitterDataBase::operator delete(void* Ptr, size_t)
{
    ::operator delete(Ptr);
}
