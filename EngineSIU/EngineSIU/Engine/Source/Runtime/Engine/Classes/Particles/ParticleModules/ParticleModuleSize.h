#pragma once
#include "ParticleModuleSizeBase.h"
#include "Distributions/DistributionVector.h"

struct FRandomStream;


class UParticleModuleSize : public UParticleModuleSizeBase
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModuleSizeBase)

    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        FRawDistributionVector, StartSize
    )

public:
    UParticleModuleSize() = default;
    virtual ~UParticleModuleSize() override = default;

    // Begin UObject Interface
    virtual void PostInitProperties() override;
    // Eng UObject Interface

    /** Initializes the default values for this property */
    void InitializeDefaults();

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;

    /**
     *  spawn 함수의 확장 버전으로, 분포 값 추출 시 랜덤 스트림을 사용할 수 있습니다.
     *
     *  @param  Owner              파티클 이미터 인스턴스(스폰을 수행하는 주체)
     *  @param  Offset             모듈 페이로드 데이터에 대한 오프셋
     *  @param  SpawnTime          스폰이 발생한 시간
     *  @param  InRandomStream     랜덤 값을 추출할 때 사용할 랜덤 스트림
     */
    void SpawnEx(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FRandomStream* InRandomStream, FBaseParticle* ParticleBase);
    virtual FName GetModuleName() const override
    {
        return FName(TEXT("Initial Size"));
    }
};
