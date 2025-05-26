#pragma once

// rigid body collision에서의 물체의 타입
enum class ECollisionChannel : uint8
{
    ECC_WorldStatic,
    ECC_WorldDynamic,
    ECC_Pawn,
    ECC_Visibility,
    ECC_Camera,
    ECC_PhysicsBody,
    ECC_Vehicle,
    ECC_MAX,
};

namespace ECollisionEnabled
{
    enum class Type : uint8
    {
        // 물리 엔진에서 어떠한 역할도 하지 않음
        NoCollision,

        // 물리 엔진에서 쿼리(raycast, query, overlap)만 수행. 물리적 상호작용(rigid body, constraint)은 없음
        QueryOnly,

        // 물리 엔진에서 물리적 상호작용만 수행. 쿼리는 없음
        PhysicsOnly,

        // 물리 엔진에서 물리적 상호작용과 쿼리를 모두 수행
        QueryAndPhysics,

        // 물리 엔진에서 시뮬레이션만 수행. 상호작용은 없음
        // 충돌 결과값은 얻지만, 실제로 자신에게 적용되진 않음.
        ProbeOnly,

        // 물리 엔진에서 시뮬레이션과 쿼리를 모두 수행
        // 충돌 결과값은 얻지만, 실제로 자신에게 적용되진 않음.
        QueryAndProbe
    };
}
