#include "Developer/PhysicsUtilities/PhysicsAssetUtils.h"
#include "ThirdParty/physx/include/extensions/PxD6Joint.h"

namespace FPhysicsAssetUtilsPhysx
{   
    // ELinearConstraintMotion을 PxD6Motion::Enum으로 매핑하는 헬퍼 함수
    physx::PxD6Motion::Enum MapLinearMotionToPx(ELinearConstraintMotion motion)
    {
        switch (motion)
        {
            case ELinearConstraintMotion::LCM_Free:   return physx::PxD6Motion::eFREE;
            case ELinearConstraintMotion::LCM_Limited:return physx::PxD6Motion::eLIMITED;
            case ELinearConstraintMotion::LCM_Locked: return physx::PxD6Motion::eLOCKED;
            default:                                  return physx::PxD6Motion::eLOCKED; // 기본값은 잠금
        }
    }
    // EAngularConstraintMotion을 PxD6Motion::Enum으로 매핑하는 헬퍼 함수
    physx::PxD6Motion::Enum MapAngularMotionToPx(EAngularConstraintMotion motion)
    {
        switch (motion)
        {
            case EAngularConstraintMotion::ACM_Free:    return physx::PxD6Motion::eFREE;
            case EAngularConstraintMotion::ACM_Limited: return physx::PxD6Motion::eLIMITED;
            case EAngularConstraintMotion::ACM_Locked:  return physx::PxD6Motion::eLOCKED;
            default:                                    return physx::PxD6Motion::eLOCKED; // 기본값은 잠금
        }
    }
}
