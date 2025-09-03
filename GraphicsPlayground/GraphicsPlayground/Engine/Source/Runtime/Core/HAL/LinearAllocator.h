#pragma once
#include <cstddef>
#include <memory>

/**
 * Allocate arbitrary size linearly in heap.
 */
class FLinearAllocator
{
public:
    explicit FLinearAllocator(size_t InitialCapacity = 64 * 1024)
        : Capacity(InitialCapacity)
    {
        Data = static_cast<std::byte*>(::operator new(Capacity));
    }

    ~FLinearAllocator()
    {
        ::operator delete(Data);
    }

    /**
     * Allocate memory with the given size and alignment.
     */
    void* Alloc(size_t Size, size_t Alignment)
    {
        size_t Current = reinterpret_cast<size_t>(Data) + Offset;
        size_t Aligned = (Current + (Alignment - 1)) & ~(Alignment - 1);
        size_t AlignedOffset = Aligned - reinterpret_cast<size_t>(Data);
        size_t NewOffset = AlignedOffset + Size;
        if (NewOffset > Capacity)
        {
            Grow(Size + Alignment);
            return Alloc(Size, Alignment); // recursive
        }
        Offset = NewOffset;
        return reinterpret_cast<void*>(Aligned);
    }

    /** (Optional) Clear memory to zero. */
    void Clear()
    {
        std::memset(Data, 0, Capacity);
    }

    /** Do not clear memory but reset offset. */
    void Reset()
    {
        Offset = 0;
    }


private:
    void Grow(size_t MinAdditional)
    {
        size_t NewCapacity = Capacity * 2;
        while (NewCapacity < Capacity + MinAdditional)
            NewCapacity *= 2;

        std::byte* NewData = static_cast<std::byte*>(::operator new(NewCapacity));
        std::memcpy(NewData, Data, Offset);
        ::operator delete(Data);
        Data = NewData;
        Capacity = NewCapacity;
    }

private:
    /** Pointer to the allocated memory */
    std::byte* Data = nullptr;
    /** Total capacity of the allocated memory */
    size_t Capacity = 0;
    /** Current offset in the allocated memory = Allocated size */
    size_t Offset = 0;
};
