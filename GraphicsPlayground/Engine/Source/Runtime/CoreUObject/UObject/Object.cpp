#include "Object.h"

#include "ObjectFactory.h"
#include "Class.h"
#include "Classes/Engine/Engine.h"
#include "Core/Math/Vector4.h"


UClass* UObject::StaticClass()
{
    static UClass ClassInfo{
        "UObject",
        sizeof(UObject),
        alignof(UObject),
        nullptr,
        []() -> UObject*
        {
            void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(sizeof(UObject));
            ::new (RawMemory) UObject;
            return static_cast<UObject*>(RawMemory);
        }
    };
    return &ClassInfo;
}

UObject::UObject()
    : UUID(0)
    // TODO: Object를 생성할 때 직접 설정하기
    , InternalIndex(-1)
    , NamePrivate("None")
{
}

UObject* UObject::Duplicate(UObject* InOuter)
{
    return FObjectFactory::ConstructObject(GetClass(), InOuter);
}

void UObject::PostInitProperties()
{
}

void UObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
}

void UObject::Serialize(FArchive& Ar)
{
    // TODO: Serialize 구현
    // GetClass()->SerializeBin(Ar, this);
}

UWorld* UObject::GetWorld() const
{
    if (UObject* Outer = GetOuter())
    {
        return Outer->GetWorld();
    }
    return nullptr;
}

void UObject::MarkAsGarbage()
{
    GUObjectArray.MarkRemoveObject(this);
}

FVector4 UObject::EncodeUUID() const
{
    FVector4 Result;

    Result.X = static_cast<float>(UUID % 0xFF);
    Result.Y = static_cast<float>(UUID >> 8 & 0xFF);
    Result.Z = static_cast<float>(UUID >> 16 & 0xFF);
    Result.W = static_cast<float>(UUID >> 24 & 0xFF);

    return Result;
}

bool UObject::IsA(const UClass* SomeBase) const
{
    const UClass* ThisClass = GetClass();
    return ThisClass->IsChildOf(SomeBase);
}
