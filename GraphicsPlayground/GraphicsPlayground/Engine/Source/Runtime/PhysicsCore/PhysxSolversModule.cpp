#include "PhysxSolversModule.h"
#include "PhysicsSolver.h"

#define PVD_HOST "127.0.0.1"
#define PX_NUM_DISPATCHER 2

// 싱글톤 인스턴스를 생성하고 초기화하는 함수
FPhysxSolversModule* FPhysxSolversModule::GetModule()
{
    static FPhysxSolversModule* Instance = nullptr;

    if (!Instance)
    {
        Instance = new FPhysxSolversModule();
        Instance->Initialize();
    }

    return Instance;
}

void FPhysxSolversModule::Initialize()
{
    Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, ErrorCallback);

    // PVD (PhysX Visual Debugger) 설정
#ifdef _DEBUG
    Pvd = PxCreatePvd(*Foundation);
    PvdTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    Pvd->connect(*PvdTransport, PxPvdInstrumentationFlag::eALL);

    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), true, Pvd);
#else
    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale());
#endif

    PxInitExtensions(*Physics, Pvd);
    DefaultMaterial = Physics->createMaterial(0.5f, 0.5f, 0.02f);
}

void FPhysxSolversModule::Shutdown()
{
    for (FPhysicsSolver* Solver : Solvers)
    {
        Solver->Release();
        delete Solver;
    }
    Solvers.Empty();
    if (PvdTransport)
    {
        Pvd->disconnect();
        PvdTransport->release();
        PvdTransport = nullptr;
    }
    if (Pvd)
    {
        Pvd->release();
        Pvd = nullptr;
    }
    if (Physics)
    {
        Physics->release();
        Physics = nullptr;
    }
    if (Foundation)
    {
        Foundation->release();
        Foundation = nullptr;
    }
}

void FPhysxSolversModule::ConnectToPVD()
{
    Pvd->connect(*PvdTransport, PxPvdInstrumentationFlag::eALL);
}

FPhysicsSolver* FPhysxSolversModule::CreateSolver()
{
    FPhysicsSolver* NewSolver = new FPhysicsSolver();

    NewSolver->Init();

    return NewSolver;
}

PxScene* FPhysxSolversModule::CreateScene()
{
    PxSceneDesc sceneDesc(Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0, 0, -10);
    Dispatcher = PxDefaultCpuDispatcherCreate(PX_NUM_DISPATCHER);
    sceneDesc.cpuDispatcher = Dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
#if _DEBUG
    sceneDesc.flags |= PxSceneFlag::eENABLE_ENHANCED_DETERMINISM;
#endif // _DEBUG

    PxScene* Scene = Physics->createScene(sceneDesc);


#ifdef _DEBUG
    Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f); // PVD용
    Scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f); // PVD용
    Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f); // PVD용
    Scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
    Scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
    Scene->getScenePvdClient()->setScenePvdFlags(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS | PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES | PxPvdSceneFlag::eTRANSMIT_CONTACTS);
#endif
    return Scene;
}
