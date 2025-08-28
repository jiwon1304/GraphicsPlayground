//#pragma once
//
//#include <PxPhysicsAPI.h>
//#include "Math/Vector.h"
//#include "Math/Quat.h"
//#include "Math/Transform.h"
//
//using PxVec3 = FVector;
//using PxQuat = FQuat;
//using PxTransform = FTransform;
//using PxRaycastHit = FHitResult;
//using PxOverlapHit = FOverlapResult;
//using PxSweepHit = FSweepResult;
//using PxFilterData = FMaskFilter;
//using PxMaterial = UPhysicalMaterial;
//using PxShape = FBodyInstance;
//using PxRigidActor = FBodyInstance;
//using PxRigidDynamic = FBodyInstance;
//using PxRigidStatic = FBodyInstance;
//using PxJoint = FConstraintInstance;
//using PxScene = UWorld->GetPhysicsScene();
//
//FBodyInstance * Instance = new FBodyInstance(OwnerComp, ...);
//PxActor* Body = gPhysics->createRigidDynamic(...);
//Body->userData = (void*)Instance;
//AActor* Actor = ((BodyInstance*)Body->userData)->OwnerComponent->GetOwner();
//
//struct FBodyInstance
//{
//    UPrimitiveComponent* OwnerComponent;
//};
