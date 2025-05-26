// Integration code for PhysX 4.1
#pragma once

#include <PxPhysicsAPI.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace physx;
using namespace DirectX;

#define SCOPED_READ_LOCK(scene) PxSceneReadLock scopedReadLock(scene);

// 게임 오브젝트
struct GameObject {
    PxRigidDynamic* rigidBody = nullptr;
    XMMATRIX worldMatrix = XMMatrixIdentity();

    void UpdateFromPhysics();
};


void InitPhysX();

GameObject CreateBox(const PxVec3& pos, const PxVec3& halfExtents);

void Simulate(float dt);

class MySimulationEventCallback : public PxSimulationEventCallback
{
public:
    void onContact(const PxContactPairHeader& pairHeader,
        const PxContactPair* pairs,
        PxU32 nbPairs) override;
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override {}
    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {}
    virtual void onWake(PxActor** actors, PxU32 count) override {}
    virtual void onSleep(PxActor** actors, PxU32 count) override {}
    virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}
};
