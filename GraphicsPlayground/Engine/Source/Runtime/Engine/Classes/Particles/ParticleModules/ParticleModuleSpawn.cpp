#include "ParticleModuleSpawn.h"

void UParticleModuleSpawn::PostInitProperties()
{
    bEnabled = true;
}

FName UParticleModuleSpawn::GetModuleName() const
{
    return FName(TEXT("Spawn"));
}
