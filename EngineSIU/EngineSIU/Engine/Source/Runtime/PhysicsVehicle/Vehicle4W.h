#pragma once

#include <ctype.h>
#pragma once

#include "PxPhysicsAPI.h"

#include "vehicle/PxVehicleUtil.h"
#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleFilterShader.h"
#include "SnippetVehicleTireFriction.h"
#include "SnippetVehicleCreate.h"
#include "Engine/Classes/PhysicsEngine/BodyInstance.h"

using namespace physx;
using namespace snippetvehicle;

class UVehicleMovementComponent;
//class FBodyInstance;

#define FVectorToPxVec3(PxV, FV) \
    PxV.x = FV.X; PxV.y = FV.Y; PxV.z = FV.Z;

enum DriveMode
{
    eDRIVE_MODE_ACCEL_FORWARDS = 0,
    eDRIVE_MODE_ACCEL_REVERSE,
    eDRIVE_MODE_HARD_TURN_LEFT,
    eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
    eDRIVE_MODE_HARD_TURN_RIGHT,
    eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
    eDRIVE_MODE_BRAKE,
    eDRIVE_MODE_NONE
};

class FVehicle4W 
{
public:
    FVehicle4W();


    PxF32 SteerVsForwardSpeedData[2 * 8] = 
    {
        0.0f,		0.75f,
        5.0f,		0.75f,
        30.0f,		0.125f,
        120.0f,		0.1f,
        PX_MAX_F32, PX_MAX_F32,
        PX_MAX_F32, PX_MAX_F32,
        PX_MAX_F32, PX_MAX_F32,
        PX_MAX_F32, PX_MAX_F32
    };

    PxFixedSizeLookupTable<8> SteerVsForwardSpeedTable;

    PxVehicleKeySmoothingData KeySmoothingData =
    {
        {
            6.0f,	//rise rate eANALOG_INPUT_ACCEL
            6.0f,	//rise rate eANALOG_INPUT_BRAKE		
            6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
            2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
            2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
        },
        {
            10.0f,	//fall rate eANALOG_INPUT_ACCEL
            10.0f,	//fall rate eANALOG_INPUT_BRAKE		
            10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
            5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
            5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
        }
    };

    PxVehiclePadSmoothingData PadSmoothingData =
    {
        {
            6.0f,	//rise rate eANALOG_INPUT_ACCEL
            6.0f,	//rise rate eANALOG_INPUT_BRAKE		
            6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
            2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
            2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
        },
        {
            10.0f,	//fall rate eANALOG_INPUT_ACCEL
            10.0f,	//fall rate eANALOG_INPUT_BRAKE		
            10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
            5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
            5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
        }
    };

    DriveMode DriveModeOrder[13] =
    {
        eDRIVE_MODE_BRAKE,
        eDRIVE_MODE_ACCEL_FORWARDS,
        eDRIVE_MODE_BRAKE,
        eDRIVE_MODE_ACCEL_REVERSE,
        eDRIVE_MODE_BRAKE,
        eDRIVE_MODE_HARD_TURN_LEFT,
        eDRIVE_MODE_BRAKE,
        eDRIVE_MODE_HARD_TURN_RIGHT,
        eDRIVE_MODE_ACCEL_FORWARDS,
        eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
        eDRIVE_MODE_ACCEL_FORWARDS,
        eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
        eDRIVE_MODE_NONE
    };

    UVehicleMovementComponent* VehicleMovementComponent;

    PxVehicleDrive4WRawInputData VehicleInputData;
    PxVehicleDrive4W* gVehicle4W = nullptr;
    PxRigidStatic* gGroundPlane = nullptr;
    PxCooking* gCooking = nullptr;
    VehicleSceneQueryData* gVehicleSceneQueryData = nullptr;
    PxBatchQuery* gBatchQuery = nullptr;
    PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = nullptr;

    bool gIsVehicleInAir = true;


    PxF32 VehicleModeLifetime = 4.0f;
    PxF32 VehicleModeTimer = 0.0f;
    PxU32 VehicleOrderProgress = 0;
    bool VehicleOrderComplete = false;
    bool MimicKeyInputs = true;

    VehicleDesc InitVehicleDesc(UVehicleMovementComponent* InVehicleMovementComponent, PxMaterial* gMaterial);

    void StartAccelerateForwardsMode();
    void StartAccelerateReverseMode();
    void StartBrakeMode();
    void StartTurnHardLeftMode();
    void StartTurnHardRightMode();
    void StartHandbrakeTurnLeftMode();
    void StartHandbrakeTurnRightMode();
    void ReleaseAllControls();

    PxActor* InitVehicle(UVehicleMovementComponent* InVehicleMovementComponent, FBodyInstance* BodyInstance, const FMatrix& InitialMatrix,
        PxPhysics* gPhysics, PxFoundation* gFoundation, PxScene* gScene, 
        PxDefaultAllocator* gAllocator, PxMaterial* gMaterial);

    void IncrementDrivingMode(const float Timestep);

    void StepPhysics(const float Timestep, PxScene* gScene);
};
