#pragma once

#include "Math/Vector.h"
#include "Container/Set.h"
#include "Classes/Engine/OverlapInfo.h"
#include "Math/Transform.h"
#include "Classes/PhysicsEngine/BodyInstance.h"

class UShapeComponent;
class IPhysicsSolver;
class AWheeledVehiclePawn;

// PxScene을 갖고 있는 Scene으로, FPhysScene은 PxScene에 대해서 알 수 없습니다.
// 오직 FPhysicsSolver만 값을 얻을 수 있습니다.
struct FPhysScene
{
	friend IPhysicsSolver;
public:
    FPhysScene() = default;
    ~FPhysScene() = default;

    virtual void Init(IPhysicsSolver* InSceneSolver);
    virtual void Release();

	void AddActor(AActor* Actor);

    //void AddVehicle(AWheeledVehiclePawn* Vehicle);

    void AdvanceAndDispatch_External(float DeltaTime);
    void SyncBodies();

    void SetGeometryToWorld(UBodySetup* BodySetup);
    void AddRigidBody(const FBodyInstance* Instance);
    void RemoveRigidBody(FBodyInstance* Component);

    // 물리 쿼리 함수 예시
    bool GetOverlappings(UShapeComponent* Shape, TArray<FOverlapInfo>& OutOverlaps);

    float DeltaTime = 0.0f;

    virtual void SetGravity(FVector InGravity) = 0;
protected:
    //TMap<FBodyInstance*, physx::PxActor*> RegisteredInstances;

    IPhysicsSolver* SceneSolver = nullptr;
    
    FVector Gravity = FVector(0,0, -98.f);
};
