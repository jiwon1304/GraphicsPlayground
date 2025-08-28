#pragma once
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Chaos/FChaosSolversModule 참고
// Engine\Source\Runtime\Experimental\Chaos\Public\ChaosSolversModule.h
#include "CoreUObject/UObject/Object.h"
#include "PhysicsCore/PhysXIntegration.h"

class FPhysicsSolver;
class FPhysScene;

class FPhysxSolversModule
{
public:
    static FPhysxSolversModule* GetModule();

    void Initialize();
    void Shutdown();
    void ConnectToPVD();

    FPhysicsSolver* CreateSolver();
    PxScene* CreateScene();

    TArray<FPhysicsSolver*> Solvers;

    // Physx
    PxDefaultAllocator      Allocator;
    PxDefaultErrorCallback  ErrorCallback;
    PxFoundation* Foundation = nullptr;
    PxPhysics* Physics = nullptr;
    PxMaterial* DefaultMaterial = nullptr;
    PxDefaultCpuDispatcher* Dispatcher = nullptr;
    PxPvd* Pvd = nullptr;
    PxPvdTransport* PvdTransport = nullptr;
};
