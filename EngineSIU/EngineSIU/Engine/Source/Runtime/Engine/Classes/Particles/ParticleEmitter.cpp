#include "ParticleEmitter.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Components/ParticleSystemComponent.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModules/ParticleModule.h"
#include "Particles/ParticleEmitterInstances.h"
#include "Particles/ParticleHelper.h"
#include "Particles/ParticleModules/ParticleModuleTypeDataBase.h"

FParticleEmitterInstance* UParticleEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    UE_LOG(ELogLevel::Error, TEXT("UParticleEmitter::CreateInstance is pure virtual"));
    return nullptr;
}

UParticleLODLevel* UParticleEmitter::GetCurrentLODLevel(FParticleEmitterInstance* Instance)
{
    // !NOTE : 지금은 LOD레벨 1개
    return Instance->CurrentLODLevel;
}

void UParticleEmitter::Build()
{
    CacheEmitterModuleInfo();
}

void UParticleEmitter::CacheEmitterModuleInfo()
{
    ParticleSize = sizeof(FBaseParticle);
    TypeDataOffset = 0;
    TypeDataInstanceOffset = -1;
    ModuleOffsetMap.Empty();
    ModuleInstanceOffsetMap.Empty();
    ModulesNeedingInstanceData.Empty();

    UParticleLODLevel* HighLODLevel = GetLODLevel(0);
    assert(HighLODLevel);

    UParticleModuleTypeDataBase* HighTypeData = HighLODLevel->TypeDataModule;
    if (HighTypeData)
    {
        uint32 ReqBytes = HighTypeData->RequiredBytes(nullptr);
        if (ReqBytes)
        {
            TypeDataOffset = ParticleSize;
            ParticleSize += ReqBytes;
        }

        int32 TempInstanceBytes = HighTypeData->RequiredBytesPerInstance();
        if (TempInstanceBytes)
        {
            TypeDataInstanceOffset = ReqInstanceBytes;
            ReqInstanceBytes += TempInstanceBytes;
        }
    }

    UParticleModuleRequired* RequiredModule = HighLODLevel->RequiredModule;
    assert(RequiredModule);

    for (int32 ModuleIdx = 0; ModuleIdx < HighLODLevel->Modules.Num(); ModuleIdx++)
    {
        UParticleModule* ParticleModule = HighLODLevel->Modules[ModuleIdx];
        assert(ParticleModule);

        if (ParticleModule->IsA<UParticleModuleTypeDataBase>() == false) // 타입관련 모듈 아니라면(위에서 했음)
        {
            int32 ReqBytes = ParticleModule->RequiredBytes(HighTypeData);
            if (ReqBytes)
            {
                ModuleOffsetMap.Add(ParticleModule, ParticleSize);
                // !TODO : 모듈들의 클래스 정보에 따라 따로 저장할 오프셋들을 지정, 저장
                ParticleSize += ReqBytes;

                int32 TempInstanceBytes = ParticleModule->RequiredBytesPerInstance();
                if (TempInstanceBytes)
                {
                    ModuleInstanceOffsetMap.Add(ParticleModule, ReqInstanceBytes);
                    ModulesNeedingInstanceData.Add(ParticleModule);

                    ReqInstanceBytes += TempInstanceBytes;

                    // NOTE : 어차피 지금 LOD 하나라 여기 동작안함
                    for (int32 LODIdx = 1; LODIdx < LODLevels.Num(); LODIdx++)
                    {
                        UParticleLODLevel* CurLODLevel = LODLevels[LODIdx];
                        ModuleInstanceOffsetMap.Add(CurLODLevel->Modules[ModuleIdx], ReqInstanceBytes);
                    }
                }
            }
        }
    }
}

UParticleLODLevel* UParticleEmitter::GetLODLevel(int32 LODLevel)
{
    if (LODLevel >= LODLevels.Num())
    {
        return nullptr;
    }
    return LODLevels[LODLevel];
}

