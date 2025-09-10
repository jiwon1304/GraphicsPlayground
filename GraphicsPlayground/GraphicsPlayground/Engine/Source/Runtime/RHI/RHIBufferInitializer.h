// #pragma once

// #include "RHI/RHICommandList.h"
// #include "RHI/RHIResources.h"

// /**
//  * Structure used to initialize buffer when creation
//  */
// struct FRHIBufferInitializer
// {
// public:
//     FRHIBufferInitializer() = default;
//     FRHIBufferInitializer(FRHIBufferInitializer&&) noexcept = default;

//     void WriteData(const void* InData, uint64 InSize)
//     {
//         assert(InSize <= WritableSize);
//         assert(WritableData);
//         FPlatformMemory::Memcpy(WritableData, InData, static_cast<size_t>(InSize));
//     }
    
//     void WriteDataParallel(uint64 Offset, const void* InData, uint64 InSize)
//     {
//         assert(Offset < WritableSize && Offset + InSize <= WritableSize);
//         assert(WritableData);
//         FPlatformMemory::Memcpy(static_cast<uint8*>(WritableData) + Offset, InData, static_cast<size_t>(InSize));
//     }

//     FBufferRHIRef Finalize()
//     {

//     }


// protected:
// 	using FFinalizeCallback = std::function<FBufferRHIRef(FRHICommandListBase&)>;

//     FFinalizeCallback FinalizeCallback = nullptr;

//     // used in the finalize
//     FRHICommandListBase* CommandList = nullptr;
    
//     /**
//      * Current RHI Buffer being initialized.
//      * Will only be used for command list validation since each RHI implementation will manage their own buffer type.
//      */
//     FRHIBuffer* Buffer = nullptr;

//     // Pointer mapped to GPU Data provided by the RHI
//     void* WritableData = nullptr;

//     uint64 WritableSize = 0;
// };
