#pragma once

#include "Container/Set.h"
#include "Math/Vector.h"
#include "Math/Transform.h"
#include "CollisionShape.h"
#include "Classes/PhysicsEngine/BodyInstance.h"
#include "Core/Delegates/DelegateCombination.h"

class AActor;
struct FHitResult;
struct FPhysicsBody;
struct FPhysScene;
struct FKShapeElem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOverlapDelegate, const FPhysicsBody&)

// Physx를 이용한 solver
// Physx와의 인터페이스로 사용됩니다.
// FPBDPhysicsSolver와 FPBDRigidsEvolutionGBF를 참고함
class IPhysicsSolver
{
public:
    IPhysicsSolver() = default;

    // 복사 생성자와 복사 대입 연산자를 delete로 명시적으로 삭제
    IPhysicsSolver(const IPhysicsSolver&) = delete;
    IPhysicsSolver& operator=(const IPhysicsSolver&) = delete;

    virtual void* RegisterObject(FPhysScene* InScene, const FBodyInstance* NewInstance, const FMatrix& InitialMatrix) = 0;
    // 자동차 생성하는 코드
    // virtual void* RegisterObject(FPhysScene* InScene, FBodyInstance* NewInstance, UVehicleMovementComponent* InVehicleMovementComponent, const FMatrix& InitialMatrix) = 0;
    
    // 물리 시뮬레이션을 특정 시간에 대해서 진행
    virtual void AdvanceOneTimeStep(FPhysScene* InScene, float Dt) = 0;

    virtual void FetchData(FPhysScene* InScene) = 0;

    virtual void* CreateJoint(
        FPhysScene* InScene,
        void* InChild,
        void* InParent,
        const FConstraintInstance* NewInstance) = 0;

protected:
    TArray<FBodyInstance*> RegisteredBodies; // 등록된 물리 객체들
    
    // TArray<FVehicle4W*> Vehicles; // Vehicle 부분 작업
};
