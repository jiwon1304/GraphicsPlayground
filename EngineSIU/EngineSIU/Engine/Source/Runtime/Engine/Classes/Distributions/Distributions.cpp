#include "Distributions.h"

#include "DistributionVector.h"
#include "UObject/Property.h"


void FRawDistribution::GetValue(float Time, float* Value, int32 NumCoords, int32 Extreme, FRandomStream* InRandomStream) const
{
    assert(NumCoords == 3 || NumCoords == 1);

    switch (LookupTable.Op)
    {
    case RDO_None:
        if (NumCoords == 1)
        {
            GetValue1None(Time, Value);
        }
        else
        {
            GetValue3None(Time, Value);
        }
        break;
    case RDO_Extreme:
        if (NumCoords == 1)
        {
            GetValue1Extreme(Time, Value, Extreme, InRandomStream);
        }
        else
        {
            GetValue3Extreme(Time, Value, Extreme, InRandomStream);
        }
        break;
    case RDO_Random:
        if (NumCoords == 1)
        {
            GetValue1Random(Time, Value, InRandomStream);
        }
        else
        {
            GetValue3Random(Time, Value, InRandomStream);
        }
        break;
    }
}

void FRawDistribution::GetValue1(float Time, float* Value, int32 Extreme, FRandomStream* InRandomStream) const
{
    switch (LookupTable.Op)
    {
    case RDO_None:
        GetValue1None(Time, Value);
        break;
    case RDO_Extreme:
        GetValue1Extreme(Time, Value, Extreme, InRandomStream);
        break;
    case RDO_Random:
        GetValue1Random(Time, Value, InRandomStream);
        break;
    default: // compiler complains
        assert(0);
        *Value = 0.0f;
        break;
    }
}

void FRawDistribution::GetValue3(float Time, float* Value, int32 Extreme, FRandomStream* InRandomStream) const
{
    switch (LookupTable.Op)
    {
    case RDO_None:
        GetValue3None(Time, Value);
        break;
    case RDO_Extreme:
        GetValue3Extreme(Time, Value, Extreme, InRandomStream);
        break;
    case RDO_Random:
        GetValue3Random(Time, Value, InRandomStream);
        break;
    }
}

void FRawDistribution::GetValue1Extreme(float Time, float* InValue, int32 Extreme, FRandomStream* InRandomStream) const
{
    float* RESTRICT Value = InValue;
    const float* Entry1;
    const float* Entry2;
    float LerpAlpha = 0.0f;
    const float RandValue = DIST_GET_RANDOM_VALUE(InRandomStream);
    LookupTable.GetEntry(Time, Entry1, Entry2, LerpAlpha);
    const float* RESTRICT NewEntry1 = Entry1;
    const float* RESTRICT NewEntry2 = Entry2;
    const int32 InitialElement = ((Extreme > 0) || ((Extreme == 0) && (RandValue > 0.5f)));
    Value[0] = FMath::Lerp(NewEntry1[InitialElement + 0], NewEntry2[InitialElement + 0], LerpAlpha);
}

void FRawDistribution::GetValue3Extreme(float Time, float* InValue, int32 Extreme, FRandomStream* InRandomStream) const
{
    float* RESTRICT Value = InValue;
    const float* Entry1;
    const float* Entry2;
    float LerpAlpha = 0.0f;
    const float RandValue = DIST_GET_RANDOM_VALUE(InRandomStream);
    LookupTable.GetEntry(Time, Entry1, Entry2, LerpAlpha);
    const float* RESTRICT NewEntry1 = Entry1;
    const float* RESTRICT NewEntry2 = Entry2;
    int32 InitialElement = ((Extreme > 0) || ((Extreme == 0) && (RandValue > 0.5f)));
    InitialElement *= 3;
    const float T0 = FMath::Lerp(NewEntry1[InitialElement + 0], NewEntry2[InitialElement + 0], LerpAlpha);
    const float T1 = FMath::Lerp(NewEntry1[InitialElement + 1], NewEntry2[InitialElement + 1], LerpAlpha);
    const float T2 = FMath::Lerp(NewEntry1[InitialElement + 2], NewEntry2[InitialElement + 2], LerpAlpha);
    Value[0] = T0;
    Value[1] = T1;
    Value[2] = T2;
}

void FRawDistribution::GetValue1Random(float Time, float* InValue, FRandomStream* InRandomStream) const
{
    float* RESTRICT Value = InValue;
    const float* Entry1;
    const float* Entry2;
    float LerpAlpha = 0.0f;
    const float RandValue = DIST_GET_RANDOM_VALUE(InRandomStream);
    LookupTable.GetEntry(Time, Entry1, Entry2, LerpAlpha);
    const float* RESTRICT NewEntry1 = Entry1;
    const float* RESTRICT NewEntry2 = Entry2;
    const float Value1 = FMath::Lerp(NewEntry1[0], NewEntry2[0], LerpAlpha);
    const float Value2 = FMath::Lerp(NewEntry1[1 + 0], NewEntry2[1 + 0], LerpAlpha);
    Value[0] = Value1 + (Value2 - Value1) * RandValue;
}

void FRawDistribution::GetValue3Random(float Time, float* InValue, FRandomStream* InRandomStream) const
{
    float* RESTRICT Value = InValue;
    const float* Entry1;
    const float* Entry2;
    float LerpAlpha = 0.0f;
    FVector RandValues;

    RandValues[0] = DIST_GET_RANDOM_VALUE(InRandomStream);
    RandValues[1] = DIST_GET_RANDOM_VALUE(InRandomStream);
    RandValues[2] = DIST_GET_RANDOM_VALUE(InRandomStream);
    switch (LookupTable.LockFlag)
    {
    case EDVLF_XY:
        RandValues.Y = RandValues.X;
        break;
    case EDVLF_XZ:
        RandValues.Z = RandValues.X;
        break;
    case EDVLF_YZ:
        RandValues.Z = RandValues.Y;
        break;
    case EDVLF_XYZ:
        RandValues.Y = RandValues.X;
        RandValues.Z = RandValues.X;
        break;
    }

    LookupTable.GetEntry(Time, Entry1, Entry2, LerpAlpha);
    const float* RESTRICT NewEntry1 = Entry1;
    const float* RESTRICT NewEntry2 = Entry2;
    const float X0 = FMath::Lerp(NewEntry1[0], NewEntry2[0], LerpAlpha);
    const float Y0 = FMath::Lerp(NewEntry1[1], NewEntry2[1], LerpAlpha);
    const float Z0 = FMath::Lerp(NewEntry1[2], NewEntry2[2], LerpAlpha);
    const float X1 = FMath::Lerp(NewEntry1[3 + 0], NewEntry2[3 + 0], LerpAlpha);
    const float Y1 = FMath::Lerp(NewEntry1[3 + 1], NewEntry2[3 + 1], LerpAlpha);
    const float Z1 = FMath::Lerp(NewEntry1[3 + 2], NewEntry2[3 + 2], LerpAlpha);
    Value[0] = X0 + (X1 - X0) * RandValues[0];
    Value[1] = Y0 + (Y1 - Y0) * RandValues[1];
    Value[2] = Z0 + (Z1 - Z0) * RandValues[2];
}

UObject* FRawDistribution::TryGetDistributionObjectFromRawDistributionProperty(FStructProperty* Property, uint8* Data)
{
    // // if the struct in this property is of type FRawDistributionFloat
    // if (Property->Struct->GetFName() == NAME_RawDistributionFloat)
    // {
    //     // then return the UDistribution pointed to by the FRawDistributionFloat
    //     return Property->ContainerPtrToValuePtr<FRawDistributionFloat>(Data)->Distribution;
    // }
    // // if the struct in this property is of type FRawDistributionVector
    // else if (Property->Struct->GetFName() == NAME_RawDistributionVector)
    // {
    //     // then return the UDistribution pointed to by the FRawDistributionVector
    //     return Property->ContainerPtrToValuePtr<FRawDistributionVector>(Data)->Distribution;
    // }

    // if this wasn't a FRawDistribution*, return NULL
    return nullptr;
}
