#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/NameTypes.h"
#include "Particles/ParticleHelper.h"

struct FParticleEmitterInstance;
struct FBaseParticle;
struct FRandomStream;

class UParticleModuleTypeDataBase;


enum EModuleType : int
{
    /** General - all emitter types can use it			*/
    EPMT_General,
    /** TypeData - TypeData modules						*/
    EPMT_TypeData,
    /** Beam - only applied to beam emitters			*/
    EPMT_Beam,
    /** Trail - only applied to trail emitters			*/
    EPMT_Trail,
    /** Spawn - all emitter types REQUIRE it			*/
    EPMT_Spawn,
    /** Required - all emitter types REQUIRE it			*/
    EPMT_Required,
    /** Event - event related modules					*/
    EPMT_Event,
    /** Light related modules							*/
    EPMT_Light,
    /** SubUV related modules							*/
    EPMT_SubUV,
    EPMT_MAX,
};

struct FParticleRandomSeedInfo
{
    DECLARE_STRUCT(FParticleRandomSeedInfo)

    /** The name to expose to the placed instances for setting this seed */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere, 
        FName, ParameterName
    )

    /**
     *	If true, the module will attempt to get the seed from the owner
     *	instance. If that fails, it will fall back to getting it from
     *	the RandomSeeds array.
     */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere, 
        bool, bGetSeedFromInstance
    )

    /**
     *	If true, the seed value retrieved from the instance will be an
     *	index into the array of seeds.
     */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere, 
        bool, bInstanceSeedIsIndex
    )

    /**
     *	If true, then reset the seed upon the emitter looping.
     *	For looping environmental effects this should likely be set to false to avoid
     *	a repeating pattern.
     */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        bool , bResetSeedOnEmitterLooping
    )

    /**
    *	If true, then randomly select a seed entry from the RandomSeeds array
    */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        bool, bRandomlySelectSeedArray
    )

    /**
     *	The random seed values to utilize for the module.
     *	More than 1 means the instance will randomly select one.
     */
    UPROPERTY_WITH_FLAGS(
        EditAnywhere,
        TArray<int32>, RandomSeeds
    )

    FParticleRandomSeedInfo()
        : bGetSeedFromInstance(false)
        , bInstanceSeedIsIndex(false)
        , bResetSeedOnEmitterLooping(true)
        , bRandomlySelectSeedArray(false)
    {
    }

    FORCEINLINE int32 GetInstancePayloadSize() const
    {
        return ((RandomSeeds.Num() > 0) ? sizeof(FParticleRandomSeedInstancePayload) : 0);
    }
};

class UParticleModule : public UObject
{
    DECLARE_CLASS(UParticleModule, UObject)

public:
    UParticleModule() = default;
    virtual ~UParticleModule() override = default;

    uint8 bSpawnModule : 1;
    uint8 bEnabled : 1;
    uint8 bUpdateModule : 1;

    virtual EModuleType GetModuleType() const { return EPMT_General; }
    /**
    * 파티클 페이로드 블록의 크기를 반환
    * 
    * @param    TypeData    이 모듈을 소유하고 있는 UParticleModuleTypeDataBase
    * @return   uint32      파티클별 이 모듈이 필요로 하는 바이트의 수
    */
    virtual uint32 RequiredBytes(UParticleModuleTypeDataBase* TypeData) const;  // should be overridden by derived classes
    virtual uint32 RequiredBytesPerInstance() const;                            // should be overridden by derived classes

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase);
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    virtual void FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    
    FRandomStream& GetRandomStream(FParticleEmitterInstance* Owner);

    virtual FName GetModuleName() const;

};
