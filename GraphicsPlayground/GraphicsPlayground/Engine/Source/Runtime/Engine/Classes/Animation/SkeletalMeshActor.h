#pragma once
#include "Engine/Classes/GameFramework/Actor.h"

class ASkeletalMeshActor : public AActor
{
    DECLARE_CLASS(ASkeletalMeshActor, AActor)

public:
    ASkeletalMeshActor() = default;
    virtual ~ASkeletalMeshActor() override = default;
};
