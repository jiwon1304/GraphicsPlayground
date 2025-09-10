#include "RHI/DynamicRHI.h"

#include <d3d11.h>

class ID3D11DynamicRHI : public FDynamicRHI
{
    virtual ID3D11Device* RHIGetDevice() const = 0;
}