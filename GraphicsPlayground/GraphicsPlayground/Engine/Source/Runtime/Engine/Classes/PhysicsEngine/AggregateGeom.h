#pragma once

#include "CoreUObject/UObject/ObjectMacros.h"
#include "ShapeElem.h"

// collision shape의 aggregate(집합체)
struct FKAggregateGeom
{
    DECLARE_STRUCT(FKAggregateGeom)

    UPROPERTY(
        EditAnywhere,
        TArray<FKSphereElem>,
        SphereElems,
    )
    UPROPERTY(
        EditAnywhere,
        TArray<FKBoxElem>,
        BoxElems,
        )
    UPROPERTY(
        EditAnywhere,
        TArray<FKSphylElem>,
        SphylElems,
        )

    friend FArchive& operator<<(FArchive& Ar, FKAggregateGeom& AggregateGeom)
    {
        return Ar << AggregateGeom.SphereElems << AggregateGeom.BoxElems << AggregateGeom.SphylElems;
    }
};
