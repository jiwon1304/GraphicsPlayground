#pragma once

#include "HAL/PlatformType.h"
#include "Core/Misc/EnumClassFlags.h"

/**
 * Properties of views which is made from this resource
 */
enum class ERHIAccess : uint32
{
	Unknown = 0,

	// CPU readback
	CPURead                 = 1 <<  0,
	// present on the screen. (bakckbuffer)
	Present                 = 1 <<  1,
	// vertex or index buffer
	VertexOrIndexBuffer     = 1 <<  2,
	// read-only for compute shader
	SRVCompute              = 1 <<  3,
	// read-only for pixel shader
	SRVGraphicsPixel        = 1 <<  4,
	// read-only for non-pixel and non-compute shader
	SRVGraphicsNonPixel     = 1 <<  5,
	// for copying to other resources
	CopySrc                 = 1 <<  6,
	// multisampled texture to resolve
	ResolveSrc              = 1 <<  7,
	// for depth stencil view
	DSVRead                 = 1 <<  8,

	// writable on compute shader
	UAVCompute              = 1 <<  9,
	// writable on graphics shader
	UAVGraphics             = 1 << 10,
	// render target view
	RTV                     = 1 << 11,
	// for copying from other resources
	CopyDest                = 1 << 12,
	// resolved from multisampling
	ResolveDst              = 1 << 13,
	// to write on DSV
	DSVWrite                = 1 << 14,

    Last = DSVWrite,
	None = Unknown,
	Mask = (Last << 1) - 1,

	// Graphics is a combination of pixel and non-pixel
	SRVGraphics = SRVGraphicsPixel | SRVGraphicsNonPixel,

	// A mask of the two possible SRV states
	SRVMask = SRVCompute | SRVGraphics,

	// A mask of the two possible DSV states
	DSVMask = DSVRead | DSVWrite,

	// A mask of the two possible UAV states
	UAVMask = UAVCompute | UAVGraphics,

	// A mask of all bits representing read-only states which cannot be combined with other write states.
	ReadOnlyExclusiveMask = CPURead | Present | VertexOrIndexBuffer | SRVGraphics | SRVCompute | CopySrc | ResolveSrc,

	// A mask of all bits representing read-only states on the compute pipe which cannot be combined with other write states.
	ReadOnlyExclusiveComputeMask = CPURead | SRVCompute | CopySrc,

	// A mask of all bits representing read-only states which may be combined with other write states.
	ReadOnlyMask = ReadOnlyExclusiveMask | DSVRead,

	// A mask of all bits representing readable states which may also include writable states.
	ReadableMask = ReadOnlyMask | UAVMask,

	// A mask of all bits representing write-only states which cannot be combined with other read states.
	WriteOnlyExclusiveMask = RTV | CopyDest | ResolveDst,

	// A mask of all bits representing write-only states which may be combined with other read states.
	WriteOnlyMask = WriteOnlyExclusiveMask | DSVWrite,

	// A mask of all bits representing writable states which may also include readable states.
	WritableMask = WriteOnlyMask | UAVMask
};
ENUM_CLASS_FLAGS(ERHIAccess)


inline constexpr bool IsReadOnlyAccess(ERHIAccess Access)
{
    return EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyMask);
}

inline constexpr bool IsWriteOnlyAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::WriteOnlyMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::WriteOnlyMask);
}

inline constexpr bool IsWritableAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::WritableMask);
}