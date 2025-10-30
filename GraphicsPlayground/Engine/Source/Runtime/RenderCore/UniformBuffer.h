#pragma once

#include "RenderResource.h"
#include "RHI/DynamicRHI.h"
#include "ShaderParameterMacros.h"

/**
 * TBufferStruct should be packed and aligned to 16 bytes.
 * This layer does not perform any validation for the structure.
 * ex) There should be 4 byte between successive members of type float3.
 */
template <typename TBufferStruct>
class TUniformBuffer : public FRenderResource
{
    static_assert(std::is_pod_v<TBufferStruct>, "TBufferStruct must be plain old data type");

    constexpr size_t Alignment = 16;
public:
    TUniformBuffer()
        : Usage(EUniformBufferUsage::Dynamic), Contents(nullptr){}
    
    virtual ~TUniformBuffer()
    {
        if (Contents)
        {
            FPlatformMemory::AlignedFree<EAT_Renderer>(Contents, sizeof(TBufferStruct));
        }
    }

    /**
     * Update the contents of the uniform buffer.
     */
    void SetContents(FRHICommandListBase& RHICmdList, const TBufferStruct& NewContents)
    {
        SetContentsNoUpdate(NewContents);
        UpdateRHI(RHICmdList);
    }

    void SetContentsToZero(FRHICommandListBase& RHICmdList)
    {
        if (!Contents)
        {
            Contents = static_cast<uint8*>(FPlatformMemory::AlignedMalloc<EAT_Renderer>(sizeof(TBufferStruct), Alignment));
        }
        FPlatformMemory::Memset(Contents, 0, sizeof(TBufferStruct));
    }

    const uint8* GetContents() const
    {
        return Contents;
    }

    // FRenderResource interface
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override
    {
		if (Contents)
		{
			UniformBufferRHI = RHICreateUniformBuffer(Contents, nullptr, EUniformBufferUsage::Dynamic);
		}
    }

    virtual void ReleaseRHI() override
    {
        UniformBufferRHI.SafeRelease();
    }

    FRHIUniformBufferRef GetUniformBufferRef() const
    {
        return UniformBufferRHI;
    }

protected:
    /**
     * for internal use
     */
    void SetContentsNoUpdate(const TBufferStruct& NewContents)
    {
        if (!Contents)
        {
            Contents = static_cast<uint8*>(FPlatformMemory::AlignedMalloc<EAT_Renderer>(sizeof(TBufferStruct), Alignment));
        }
        FPlatformMemory::Memcpy(Contents, &NewContents, sizeof(TBufferStruct));
    }
    
    EUniformBufferUsage Usage = EUniformBufferUsage::Dynamic;

private:
    FUniformBufferRHIRef UniformBufferRHI;
    uint8* Contents;
};


// TODO : Implement uniform buffer declaration macro
// Engine/Source/Runtime/RenderCore/Public/ShaderParameterMacros.h
// Engine/Source/Runtime/RenderCore/Public/ShaderParameterMetadata.h