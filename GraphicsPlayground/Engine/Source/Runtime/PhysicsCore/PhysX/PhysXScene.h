#include "PhysicsCore/PhysicsScene.h"
#include "ThirdParty/physx/include/PxScene.h"

class FPhysXSolver;

struct FPhysXScene : public FPhysScene
{
friend class FPhysXSolver;
public:
    virtual void Init(IPhysicsSolver* InSceneSolver) override;
    virtual void Release() override;

    virtual void SetGravity(FVector InGravity) override;

private:
    // PxActor -> FBodyInstance는 PxActor->usrerData를 통해서 가져올 수 있습니다.

    physx::PxScene* PhysxScene = nullptr;
};