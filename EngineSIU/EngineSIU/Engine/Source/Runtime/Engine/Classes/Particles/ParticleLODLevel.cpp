#include "ParticleLODLevel.h"
#include "Particles/ParticleModules/ParticleModuleSpawn.h"
#include "Particles/ParticleModules/ParticleModuleRequired.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"

void UParticleLODLevel::Initialize()
{
    RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(GetOuter());
}

void UParticleLODLevel::UpdateModuleLists()
{
    SpawningModules.Empty();
    SpawnModules.Empty();
    UpdateModules.Empty();

    UParticleModule* Module = 0;
    int32 TypeDataModuleIndex = -1;

    for (int32 i = 0; i < Modules.Num(); i++)
    {
        Module = Modules[i];
        if (!Module)
        {
            continue;
        }

        if (Module->bSpawnModule)
        {
            SpawnModules.Add(Module);
        }

        if (Module->bUpdateModule)
        {
            UpdateModules.Add(Module);
        }

        if (Module->IsA<UParticleModuleTypeDataBase>())
        {
            TypeDataModule = Cast<UParticleModuleTypeDataBase>(Module);
            if (TypeDataModule)
            {
                TypeDataModuleIndex = i;
            }
        }
        else if (Module->IsA<UParticleModuleSpawnBase>())
        {
            UParticleModuleSpawnBase* SpawnBase = Cast<UParticleModuleSpawnBase>(Module);
            SpawningModules.Add(SpawnBase);
        }
    }

    if (TypeDataModuleIndex != -1)
    {
        Modules.RemoveAt(TypeDataModuleIndex);
    }
}

int32 UParticleLODLevel::GetModuleIndex(UParticleModule* InModule)
{
    return int32();
}
