#include "PhysicsCore/PhysicsSolver.h"

class FPhysXSolver : public IPhysicsSolver
{
    friend class FPhysxSolversModule;
public:
    FPhysXSolver() = default;

    virtual void* RegisterObject(
        FPhysScene* InScene,
        const FBodyInstance* NewInstance,
        const FMatrix& InitialMatrix) override;

    // virtual void* RegisterObject(
    //     FPhysScene* InScene,
    //     FBodyInstance* NewInstance,
    //     UVehicleMovementComponent* InVehicleMovementComponent,
    //     const FMatrix& InitialMatrix) override;

    virtual void AdvanceOneTimeStep(FPhysScene* InScene, float Dt) override;

    virtual void FetchData(FPhysScene* InScene) override;

    void* CreateJoint(
        FPhysScene* InScene,
        void* InChild,
        void* InParent,
        const FConstraintInstance* NewInstance);
};
