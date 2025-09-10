#include "RHIResources.h"

// not thread-safe!
TQueue<FRHIResource*> PendingDeletes;

/** ETextureCreateFlags to ERHIAccess */
ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage)
{
    if(EnumHasAnyFlags(InUsage, ETextureCreateFlags::RenderTargetable))
    {
        return ERHIAccess::RTV;
    }
    else if(EnumHasAnyFlags(InUsage, ETextureCreateFlags::DepthStencilTargetable))
    {
        return ERHIAccess::DSVRead | ERHIAccess::DSVWrite;
    }
    else if(EnumHasAnyFlags(InUsage, ETextureCreateFlags::UAV))
    {
        return ERHIAccess::UAVMask;
    }
    else if(EnumHasAnyFlags(InUsage, ETextureCreateFlags::ShaderResource))
    {
        return ERHIAccess::SRVMask;
    }
    else
    {
        return ERHIAccess::SRVGraphics;
    }
}

/** EBufferUsageFlags to ERHIAccess */
ERHIAccess RHIGetDefaultResourceState(EBufferUsageFlags InUsage/*, bool bInHasInitialData*/)
{
	ERHIAccess ResourceState = ERHIAccess::Unknown;
    
    /**
     * Usage check
     */
    constexpr EBufferUsageFlags VertexIndexBuffer = EBufferUsageFlags::VertexBuffer | EBufferUsageFlags::IndexBuffer;
    if (EnumHasAnyFlags(InUsage, VertexIndexBuffer))
	{
		ResourceState = ERHIAccess::VertexOrIndexBuffer;
	}
    
    // we do not care uniform buffer
    // constexpr EBufferUsageFlags UniformBuffer = EBufferUsageFlags::UniformBuffer;
    // if (EnumHasAnyFlags(InUsage, UniformBuffer))
    // {
    //     ResourceState = ResourceState | ERHIAccess::;
    // }
    
    constexpr EBufferUsageFlags SRV = EBufferUsageFlags::ShaderResource;
	if (EnumHasAnyFlags(InUsage, SRV))
	{
		ResourceState = ResourceState | ERHIAccess::SRVMask;
	}

    constexpr EBufferUsageFlags UAV = EBufferUsageFlags::UnorderedAccess;
    if (EnumHasAnyFlags(InUsage, UAV))
    {
        ResourceState = ResourceState | ERHIAccess::UAVMask;
    }
    
	return ResourceState;
}

void FRHIResource::MarkForDelete()
{
    PendingDeletes.Enqueue(this);
}