#include "BodySetup.h"

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/ObjectFactory.h"


FArchive& operator<<(FArchive& Ar, UBodySetup*& BodySetup)
{
    if (Ar.IsLoading())
    {
        if (BodySetup == nullptr)
        {
            BodySetup = FObjectFactory::ConstructObject<UBodySetup>(nullptr);
        }
    }
        
    Ar << *static_cast<UBodySetupCore*>(BodySetup);

    Ar << BodySetup->AggGeom << BodySetup->DefaultInstance << BodySetup->PhysMaterial << BodySetup->BuildScale3D;

    if (Ar.IsLoading())
    {
        BodySetup->DefaultInstance.ExternalCollisionProfileBodySetup = BodySetup;
    }

    return Ar;
}
