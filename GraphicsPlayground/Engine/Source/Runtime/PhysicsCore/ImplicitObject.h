#pragma once
#include "CoreUObject/UObject/ObjectMacros.h"

// FKShapeElem 쓰기
//namespace ImplicitObjectType
//{
//    enum
//    {
//        //Note: add entries in order to avoid serialization issues (but before IsInstanced)
//        Sphere = 0, // warning: code assumes that this is an FSphere, but all TSpheres will think this is their type.
//        Box,
//        Plane,
//        Capsule,
//    };
//
//    using EImplicitObjectType = uint8;
//}
//struct FImplicitObject
//{
//    DECLARE_STRUCT(FImplicitObject)
//
//public:
//    bool bDoCollide;
//    bool bHasBoundingBox;
//    bool bIsConvex;
//    ImplicitObjectType::EImplicitObjectType CollisionType;
//    ImplicitObjectType::EImplicitObjectType Type;
//};
//
//struct FBoxFloat3 : public FImplicitObject
//{
//    DECLARE_STRUCT(FBoxFloat3, FImplicitObject)
//
//    FVector MMin, MMax;
//};
//
//struct FSphere : public FImplicitObject
//{
//    DECLARE_STRUCT(FSphere, FImplicitObject)
//    UPROPERTY(
//        EditAnywhere,
//        FVector,
//        Center,
//        = FVector::ZeroVector
//    )
//    UPROPERTY(
//        EditAnywhere,
//        float,
//        Radius,
//        = 1.0f
//    )
//};
