#include "ObjectUtils.h"
#include "CoreUObject/UObject/UObjectArray.h"


bool IsValid(const UObject* Test)
{
    return Test && GUObjectArray.GetObjectItemArrayUnsafe().Contains(const_cast<UObject*>(Test));
}
