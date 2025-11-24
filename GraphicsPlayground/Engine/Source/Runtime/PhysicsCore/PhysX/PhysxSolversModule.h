#pragma once
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Chaos/FChaosSolversModule 참고
// Engine\Source\Runtime\Experimental\Chaos\Public\ChaosSolversModule.h
#include "CoreUObject/UObject/Object.h"
#include "Container/Array.h"
#include "ThirdParty/physx/include/PxPhysicsAPI.h"

class IPhysicsSolver;
struct FPhysScene;

class FPhysxSolversModule
{
public:
    static FPhysxSolversModule* GetModule();

    void Initialize();
    void Shutdown();
    void ConnectToPVD();

    IPhysicsSolver* CreateSolver();
    physx::PxScene* CreateScene();

    TArray<IPhysicsSolver*> Solvers;

    // Physx
    physx::PxDefaultAllocator      Allocator;
    physx::PxDefaultErrorCallback  ErrorCallback;
    physx::PxFoundation* Foundation = nullptr;
    physx::PxPhysics* Physics = nullptr;
    physx::PxMaterial* DefaultMaterial = nullptr;
    physx::PxDefaultCpuDispatcher* Dispatcher = nullptr;
    physx::PxPvd* Pvd = nullptr;
    physx::PxPvdTransport* PvdTransport = nullptr;
};
