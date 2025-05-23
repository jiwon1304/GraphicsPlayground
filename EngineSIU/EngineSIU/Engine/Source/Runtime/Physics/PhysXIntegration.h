// Integration code for PhysX 4.1
#pragma once

#include <PxPhysicsAPI.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace physx;
using namespace DirectX;

// PhysX 전역
extern PxDefaultAllocator      gAllocator;
extern PxDefaultErrorCallback  gErrorCallback;
extern PxFoundation* gFoundation;
extern PxPhysics* gPhysics;
extern PxScene* gScene;
extern PxMaterial* gMaterial;
extern PxDefaultCpuDispatcher* gDispatcher;

// 게임 오브젝트
struct GameObject {
    PxRigidDynamic* rigidBody = nullptr;
    XMMATRIX worldMatrix = XMMatrixIdentity();

    void UpdateFromPhysics();
};

extern std::vector<GameObject> gObjects;

void InitPhysX();

GameObject CreateBox(const PxVec3& pos, const PxVec3& halfExtents);

void Simulate(float dt);

// 렌더링은 생략 – worldMatrix를 사용해 D3D11에서 월드 행렬로 렌더링

//int main() {
//    InitPhysX();
//
//    // 박스 생성
//    gObjects.push_back(CreateBox(PxVec3(0, 5, 0), PxVec3(1, 1, 1)));
//
//    // 메인 루프 예시
//    while (true) {
//        Simulate(1.0f / 60.0f);
//        // Render(gObjects[i].worldMatrix); // ← 너의 렌더링 코드에 맞춰 사용
//    }
//
//    return 0;
//}
