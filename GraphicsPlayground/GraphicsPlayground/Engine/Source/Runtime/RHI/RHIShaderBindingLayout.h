#include "Container/String.h"
#include "Container/Array.h"

using FRHIUniformBufferShaderBindingLayout = FString;

/** Uniform buffer name container */
class FRHIShaderBindingLayout
{
public:
	constexpr static uint32 MaxUniformBufferEntries = 8;

	FRHIShaderBindingLayout() = default;
	
	FRHIShaderBindingLayout(const TArray<FRHIUniformBufferShaderBindingLayout>& InUniformBufferEntries)
	{
		NumUniformBufferEntries = InUniformBufferEntries.Num();
		assert(NumUniformBufferEntries < MaxUniformBufferEntries);
        UniformBufferEntries.SetNum(NumUniformBufferEntries);
		for (uint32 Index = 0; Index < NumUniformBufferEntries; ++Index)
		{
			UniformBufferEntries[Index] = InUniformBufferEntries[Index];
		}
	}

	uint32 GetNumUniformBufferEntries() const { return NumUniformBufferEntries; }
	const FRHIUniformBufferShaderBindingLayout& GetUniformBufferEntry(uint32 Index) const
    {
        assert(Index < NumUniformBufferEntries);
        return UniformBufferEntries[Index];
    }
	   	
	const FRHIUniformBufferShaderBindingLayout* FindEntry(const FString& LayoutName) const
	{
		for (const FRHIUniformBufferShaderBindingLayout& Entry : UniformBufferEntries)
		{
			if (Entry == LayoutName)
			{
				return &Entry;
			}
		}

		return nullptr;
	}
	
	uint32 NumUniformBufferEntries = 0;
    TArray<FRHIUniformBufferShaderBindingLayout, TInlineAllocator<FRHIUniformBufferShaderBindingLayout, MaxUniformBufferEntries>> UniformBufferEntries;
};