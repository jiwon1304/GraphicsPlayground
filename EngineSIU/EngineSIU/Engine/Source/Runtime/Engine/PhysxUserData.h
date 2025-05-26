#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"

struct FKShapeElem;

namespace EPhysxUserDataType
{
    enum Type
    {
        Invalid,
        BodyInstance,
        PhysicalMaterial,
        PhysScene,
        ConstraintInstance,
        PrimitiveComponent,
        AggShape,
        CustomPayload,
    };
};

struct FBodyInstance;
class UPhysicalMaterial;
class FPhysScene;
struct FConstraintInstance;
class UPrimitiveComponent;
struct FKShapeElem;
struct FCustomPhysXPayload
{
    DECLARE_STRUCT(FCustomPhysXPayload)
    // 사용자 정의 페이로드를 위한 구조체
    void* Data;
};

struct FPhysxUserData
{
protected:
    EPhysxUserDataType::Type	Type;
    void*                       Payload;

public:
    FPhysxUserData() :Type(EPhysxUserDataType::Invalid), Payload(nullptr) {}
    FPhysxUserData(FBodyInstance* InPayload) :Type(EPhysxUserDataType::BodyInstance), Payload(InPayload) {}
    FPhysxUserData(UPhysicalMaterial* InPayload) :Type(EPhysxUserDataType::PhysicalMaterial), Payload(InPayload) {}
    FPhysxUserData(FPhysScene* InPayload) :Type(EPhysxUserDataType::PhysScene), Payload(InPayload) {}
    FPhysxUserData(FConstraintInstance* InPayload) :Type(EPhysxUserDataType::ConstraintInstance), Payload(InPayload) {}
    FPhysxUserData(UPrimitiveComponent* InPayload) :Type(EPhysxUserDataType::PrimitiveComponent), Payload(InPayload) {}
    FPhysxUserData(FKShapeElem* InPayload) :Type(EPhysxUserDataType::AggShape), Payload(InPayload) {}
    FPhysxUserData(FCustomPhysXPayload* InPayload) :Type(EPhysxUserDataType::CustomPayload), Payload(InPayload) {}

    template <class T> static T* Get(void* UserData);
    template <class T> static void Set(void* UserData, T* Payload);

    //helper function to determine if userData is garbage (maybe dangling pointer)
    static bool IsGarbage(void* UserData) { return ((FPhysxUserData*)UserData)->Type < EPhysxUserDataType::Invalid || ((FPhysxUserData*)UserData)->Type > EPhysxUserDataType::CustomPayload; }
};
