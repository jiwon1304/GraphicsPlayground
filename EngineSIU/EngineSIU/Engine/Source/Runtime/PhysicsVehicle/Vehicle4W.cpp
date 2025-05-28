#include "Vehicle4W.h"
#include "Engine/Classes/PhysicsEngine/Vehicle/VehicleMovementComponent.h"


FVehicle4W::FVehicle4W()
    : SteerVsForwardSpeedTable(SteerVsForwardSpeedData, 4)
{

}

VehicleDesc FVehicle4W::InitVehicleDesc(UVehicleMovementComponent* InVehicleMovementComponent, PxMaterial* gMaterial)
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    const float chassisMass = InVehicleMovementComponent->ChassisMass;
    PxVec3 chassisDims;
    FVector& InChassisBoxExtents = InVehicleMovementComponent->ChassisBoxExtents;
    chassisDims.x = InChassisBoxExtents.X; chassisDims.y = InChassisBoxExtents.Y; chassisDims.z = InChassisBoxExtents.Z;

    const PxVec3 chassisMOI
    ((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
        (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
        (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);

    PxVec3 chassisCMOffset;
    FVector& InChassisCMOffeset = InVehicleMovementComponent->ChassisCMOffset;
    chassisCMOffset.x = InChassisCMOffeset.X; chassisCMOffset.y = InChassisCMOffeset.Y; chassisCMOffset.z = InChassisCMOffeset.Z;

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass = InVehicleMovementComponent->WheelMass;
    const PxF32 wheelRadius = InVehicleMovementComponent->WheelRadius;
    const PxF32 wheelWidth = InVehicleMovementComponent->WheelWidth;
    const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
    const PxU32 nbWheels = InVehicleMovementComponent->NBWheels;

    VehicleDesc vehicleDesc;

    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = gMaterial;
    vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = gMaterial;
    vehicleDesc.wheelSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

    return vehicleDesc;
}

void FVehicle4W::StartAccelerateForwardsMode()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalAccel(true);
    }
    else
    {
        VehicleInputData.setAnalogAccel(1.0f);
    }
}

void FVehicle4W::StartAccelerateReverseMode()
{
    gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalAccel(true);
    }
    else
    {
        VehicleInputData.setAnalogAccel(1.0f);
    }
}

void FVehicle4W::StartBrakeMode()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalBrake(true);
    }
    else
    {
        VehicleInputData.setAnalogBrake(1.0f);
    }
}

void FVehicle4W::StartTurnHardLeftMode()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalAccel(true);
        VehicleInputData.setDigitalSteerLeft(true);
    }
    else
    {
        VehicleInputData.setAnalogAccel(true);
        VehicleInputData.setAnalogSteer(-1.0f);
    }
}

void FVehicle4W::StartTurnHardRightMode()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalAccel(true);
        VehicleInputData.setDigitalSteerRight(true);
    }
    else
    {
        VehicleInputData.setAnalogAccel(1.0f);
        VehicleInputData.setAnalogSteer(1.0f);
    }
}

void FVehicle4W::StartHandbrakeTurnLeftMode()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalSteerLeft(true);
        VehicleInputData.setDigitalHandbrake(true);
    }
    else
    {
        VehicleInputData.setAnalogSteer(-1.0f);
        VehicleInputData.setAnalogHandbrake(1.0f);
    }
}

void FVehicle4W::StartHandbrakeTurnRightMode()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalSteerRight(true);
        VehicleInputData.setDigitalHandbrake(true);
    }
    else
    {
        VehicleInputData.setAnalogSteer(1.0f);
        VehicleInputData.setAnalogHandbrake(1.0f);
    }
}

void FVehicle4W::ReleaseAllControls()
{
    if (MimicKeyInputs)
    {
        VehicleInputData.setDigitalAccel(false);
        VehicleInputData.setDigitalSteerLeft(false);
        VehicleInputData.setDigitalSteerRight(false);
        VehicleInputData.setDigitalBrake(false);
        VehicleInputData.setDigitalHandbrake(false);
    }
    else
    {
        VehicleInputData.setAnalogAccel(0.0f);
        VehicleInputData.setAnalogSteer(0.0f);
        VehicleInputData.setAnalogBrake(0.0f);
        VehicleInputData.setAnalogHandbrake(0.0f);
    }
}

PxActor* FVehicle4W::InitVehicle(UVehicleMovementComponent* InVehicleMovementComponent, FBodyInstance* BodyInstance, const FMatrix& InitialMatrix,
    PxPhysics* gPhysics, PxFoundation* gFoundation, PxScene* gScene, 
    PxDefaultAllocator* gAllocator, PxMaterial* gMaterial)
{

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient) 
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

    // Vehicle 시작
    VehicleMovementComponent = InVehicleMovementComponent;

    PxInitVehicleSDK(*gPhysics);
    PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
    PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, *gAllocator);
    gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

    //Create the friction table for each combination of tire and surface type.
    gFrictionPairs = createFrictionPairs(gMaterial);

    //Create a plane to drive on.
    PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
    gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
    gScene->addActor(*gGroundPlane);

    //Create a vehicle that will drive on the plane.
    VehicleDesc vehicleDesc = InitVehicleDesc(InVehicleMovementComponent, gMaterial);
    PxVec3 FLOffset;
    FVectorToPxVec3(FLOffset, InVehicleMovementComponent->FrontLeftWheelOffset)
    PxVec3 FROffset;
    FVectorToPxVec3(FROffset, InVehicleMovementComponent->FrontRightWheelOffset)
    PxVec3 RLOffset;
    FVectorToPxVec3(RLOffset, InVehicleMovementComponent->RearLeftWheelOffset)
    PxVec3 RROffset;
    FVectorToPxVec3(RROffset, InVehicleMovementComponent->RearRightWheelOffset)
    
    gVehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking, FLOffset, FROffset, RLOffset, RROffset, 
        InVehicleMovementComponent->PeakTorque, InVehicleMovementComponent->MaxOmega, InVehicleMovementComponent->ClutchStrength);

    PxRigidDynamic* RigidDynamic = gVehicle4W->getRigidDynamicActor();
    RigidDynamic->userData = (void*)BodyInstance;

    // --- 초기 자세 설정 시작 ---

    FVector InitialPosition = InitialMatrix.GetTranslationVector();

    // 차량을 눕히기 위한 회전 값 설정
    float angle =  PxPi / 2.0f;
    FVector axis(1.0f, 0.0f, 0.0f);
    FQuat CarRotation(axis, angle);

    BodyInstance->InvPhysXQuat = CarRotation.Inverse();

    PxVec3 Position(InitialPosition.X, InitialPosition.Y, InitialPosition.Z);
    PxQuat Rotation(CarRotation.X, CarRotation.Y, CarRotation.Z, CarRotation.W);

    PxTransform InitialTransform(Position, Rotation);
    // --- 초기 자세 설정 끝 ---

    RigidDynamic->setGlobalPose(InitialTransform);

    gScene->addActor(*RigidDynamic);

    //Set the vehicle to rest in first gear.
    //Set the vehicle to use auto-gears.
    gVehicle4W->setToRestState();
    gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    gVehicle4W->mDriveDynData.setUseAutoGears(true);

    VehicleModeTimer = 0.0f;
    VehicleOrderProgress = 0;
    StartBrakeMode();

    return gVehicle4W->getRigidDynamicActor();
}

void FVehicle4W::IncrementDrivingMode(const float Timestep)
{
    VehicleModeTimer += Timestep;
    if (VehicleModeTimer > VehicleModeLifetime)
    {
        //If the mode just completed was eDRIVE_MODE_ACCEL_REVERSE then switch back to forward gears.
        if (eDRIVE_MODE_ACCEL_REVERSE == DriveModeOrder[VehicleOrderProgress])
        {
            gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
        }

        //Increment to next driving mode.
        VehicleModeTimer = 0.0f;
        VehicleOrderProgress++;
        ReleaseAllControls();

        //If we are at the end of the list of driving modes then start again.
        if (eDRIVE_MODE_NONE == DriveModeOrder[VehicleOrderProgress])
        {
            VehicleOrderProgress = 0;
            VehicleOrderComplete = true;
        }

        //Start driving in the selected mode.
        DriveMode eDriveMode = DriveModeOrder[VehicleOrderProgress];
        switch (eDriveMode)
        {
        case eDRIVE_MODE_ACCEL_FORWARDS:
            StartAccelerateForwardsMode();
            break;
        case eDRIVE_MODE_ACCEL_REVERSE:
            StartAccelerateReverseMode();
            break;
        case eDRIVE_MODE_HARD_TURN_LEFT:
            StartTurnHardLeftMode();
            break;
        case eDRIVE_MODE_HANDBRAKE_TURN_LEFT:
            StartHandbrakeTurnLeftMode();
            break;
        case eDRIVE_MODE_HARD_TURN_RIGHT:
            StartTurnHardRightMode();
            break;
        case eDRIVE_MODE_HANDBRAKE_TURN_RIGHT:
            StartHandbrakeTurnRightMode();
            break;
        case eDRIVE_MODE_BRAKE:
            StartBrakeMode();
            break;
        case eDRIVE_MODE_NONE:
            break;
        };

        //If the mode about to start is eDRIVE_MODE_ACCEL_REVERSE then switch to reverse gears.
        if (eDRIVE_MODE_ACCEL_REVERSE == DriveModeOrder[VehicleOrderProgress])
        {
            gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
        }
    }
}

void FVehicle4W::StepPhysics(const float Timestep, PxScene* gScene)
{
    //Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
    IncrementDrivingMode(Timestep);

    //Update the control inputs for the vehicle.
    if (MimicKeyInputs)
    {
        PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(KeySmoothingData, SteerVsForwardSpeedTable, VehicleInputData, Timestep, gIsVehicleInAir, *gVehicle4W);
    }
    else
    {
        PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(PadSmoothingData, SteerVsForwardSpeedTable, VehicleInputData, Timestep, gIsVehicleInAir, *gVehicle4W);
    }

    //Raycasts.
    PxVehicleWheels* vehicles[1] = { gVehicle4W };
    PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

    //Vehicle update.
    const PxVec3 grav = gScene->getGravity();
    PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
    PxVehicleUpdates(Timestep, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

    //Work out if the vehicle is in the air.
    gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
}
