#pragma once
#include <cstddef>
#include <memory>

class FLinearAllocator
{
public:
    /** 기본 크기를 64KB */
    explicit FLinearAllocator(size_t InitialCapacity = 64 * 1024)
        : Capacity(InitialCapacity)
    {
        Data = static_cast<std::byte*>(::operator new(Capacity));
    }

    ~FLinearAllocator()
    {
        ::operator delete(Data);
    }

    void* Alloc(size_t Size, size_t Alignment)
    {
        size_t Current = reinterpret_cast<size_t>(Data) + Offset;
        size_t Aligned = (Current + (Alignment - 1)) & ~(Alignment - 1);
        size_t NewOffset = (Aligned - reinterpret_cast<size_t>(Data)) + Size;
        if (NewOffset > Capacity)
        {
            Grow(Size + Alignment);
            return Alloc(Size, Alignment); // 재귀 재시도
        }
        Offset = NewOffset;
        return reinterpret_cast<void*>(Aligned);
    }

    // Clear memory to zero
    void Clear()
    {
        std::memset(Data, 0, Capacity);
    }
    
    // do not clear memory but reset offset.
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
    std::byte* Data = nullptr;
    size_t Capacity = 0;
    size_t Offset = 0;
};
