#include "PhysicsConstraintTemplate.h"

#include "UObject/ObjectFactory.h"

FArchive& operator<<(FArchive& Ar, UPhysicsConstraintTemplate*& PhysicsConstraintTemplate)
{
    if (Ar.IsLoading())
    {
        if (!PhysicsConstraintTemplate)
        {
            PhysicsConstraintTemplate = FObjectFactory::ConstructObject<UPhysicsConstraintTemplate>(nullptr);
        }
    }
        
    return Ar << PhysicsConstraintTemplate->DefaultInstance << PhysicsConstraintTemplate->ProfileHandles;
}
