#include "PhysXScene.h"
#include "PhysxSolversModule.h"

void FPhysXScene::Init(IPhysicsSolver *InSceneSolver)
{
    FPhysScene::Init(InSceneSolver);

    // PhysX PxScene 생성
    PhysxScene = FPhysxSolversModule::GetModule()->CreateScene();
}

void FPhysXScene::Release()
{
    if (PhysxScene)
    {
        PhysxScene->release();
    }

    FPhysScene::Release();
}

void FPhysXScene::SetGravity(FVector InGravity)
{
    if (PhysxScene)
    {
        physx::PxVec3 PxGravity;
        PxGravity.x = InGravity.X;
        PxGravity.y = InGravity.Y;
        PxGravity.z = InGravity.Z;

        PhysxScene->setGravity(PxGravity);
        Gravity = InGravity;
    }
}
