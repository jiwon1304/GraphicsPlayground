#pragma once

#include "Math/Vector.h"
#include "Container/Set.h"
#include "Engine/OverlapInfo.h"
#include "Math/Transform.h"
#include "Classes/PhysicsEngine/BodyInstance.h"

class UShapeComponent;
struct FPhysicsSolver;

namespace physx
{
    class PxScene;
	class PxActor;
}

// PxScene을 갖고 있는 Scene으로, FPhysScene은 PxScene에 대해서 알 수 없습니다.
// 오직 FPhysicsSolver만 값을 얻을 수 있습니다.
struct FPhysScene
{
	friend FPhysicsSolver;
    DECLARE_STRUCT(FPhysScene)
public:
    FPhysScene() = default;
    ~FPhysScene() = default;

    void Init(FPhysicsSolver* InSceneSolver, physx::PxScene* InScene);
    void Release();

	void AddActor(AActor* Actor);

    void AdvanceAndDispatch_External(float DeltaTime);
    void SyncBodies();

    void SetGeometryToWorld(UBodySetup* BodySetup);
    void AddRigidBody(const FBodyInstance* Instance);
    void RemoveRigidBody(FBodyInstance* Component);

    // 물리 쿼리 함수 예시
    bool GetOverlappings(UShapeComponent* Shape, TArray<FOverlapInfo>& OutOverlaps);

    float DeltaTime = 0.0f;

    FVector Gravity = FVector(0,0,-98.f);
    void SetGravity(FVector InGravity);

    FPhysicsSolver* GetSolver() const { return SceneSolver; }
private:
    // PxActor -> FBodyInstance는 PxActor->usrerData를 통해서 가져올 수 있습니다.
    //TMap<FBodyInstance*, physx::PxActor*> RegisteredInstances;
    
    FPhysicsSolver* SceneSolver = nullptr;

	// 원래는 시뮬레이트 되는 오브젝트는 FPBDRigidsSOAs가 들고있지만,
	// 여기선 Physx에서 기본으로 제공하는 Scene을 사용함.
    physx::PxScene* PhysxScene = nullptr; // 소유만 하고 처리는 모두 SolversModule에서 처리
};
