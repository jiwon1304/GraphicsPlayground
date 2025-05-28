#pragma once

#include "Container/Set.h"
#include "Math/Vector.h"
#include "Math/Transform.h"
#include "CollisionShape.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysXIntegration.h"


class AActor;
struct FHitResult;
struct FPhysicsBody;
class FPhysScene;
class FKShapeElem;
class UVehicleMovementComponent;
class FVehicle4W;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOverlapDelegate, const FPhysicsBody&)

// Physx를 이용한 solver
// Physx와의 인터페이스로 사용됩니다.
// FPBDPhysicsSolver와 FPBDRigidsEvolutionGBF를 참고함
class FPhysicsSolver
{
    friend class FPhysxSolversModule;
public:
    FPhysicsSolver() = default;
    ~FPhysicsSolver() = default;

    void InitScene(FPhysScene* InScene) const;

    // 복사 생성자와 복사 대입 연산자를 delete로 명시적으로 삭제
    FPhysicsSolver(const FPhysicsSolver&) = delete;
    FPhysicsSolver& operator=(const FPhysicsSolver&) = delete;

    PxActor* RegisterObject(FPhysScene* InScene, const FBodyInstance* NewInstance);
    // 자동차 생성하는 코드
    PxActor* RegisterObject(FPhysScene* InScene, const FBodyInstance* NewInstance, UVehicleMovementComponent* InVehicleMovementComponent);
    
    // 시뮬레이션 이전 최신값을 반영

    // 물리 시뮬레이션을 특정 시간에 대해서 진행
    void AdvanceOneTimeStep(FPhysScene* InScene, float Dt);

    void FetchData(FPhysScene* InScene);

    PxGeometryType::Enum GetPxType(const FKShapeElem* InShape);
private:

protected:
    void Init();
    void Release();

    TArray<FBodyInstance*> RegisteredBodies; // 등록된 물리 객체들
    
    TArray<FVehicle4W*> Vehicles; // Vehicle 부분 작업
};
