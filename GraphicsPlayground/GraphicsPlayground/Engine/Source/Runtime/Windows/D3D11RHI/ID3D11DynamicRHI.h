#pragma once
#include "RHI/DynamicRHI.h"

struct ID3D11Device;

class ID3D11DynamicRHI : public FDynamicRHI
{
    virtual ID3D11Device* RHIGetDevice() const = 0;
};