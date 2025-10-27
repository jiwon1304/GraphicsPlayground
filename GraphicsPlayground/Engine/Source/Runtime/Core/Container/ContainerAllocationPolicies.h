#pragma once
#include <cstddef>
#include <memory>
#include <type_traits>
#include <cassert>
#include <new>

template <typename T, int NumInlineElements, int IndexSize = 32>
class TSizedInlineAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::make_unsigned_t<std::conditional_t<IndexSize == 64, int64_t, int32_t>>;
    using SizeType = size_type;
    using difference_type = std::make_signed_t<std::conditional_t<IndexSize == 64, int64_t, int32_t>>;

    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;

    TSizedInlineAllocator() noexcept : m_used(false) {}
    template <typename U> TSizedInlineAllocator(const TSizedInlineAllocator<U, NumInlineElements, IndexSize>&) noexcept : m_used(false) {}

    ~TSizedInlineAllocator() = default;

    pointer allocate(size_type n) {
        if (!m_used && n <= NumInlineElements) {
            m_used = true;
            return reinterpret_cast<pointer>(&m_buffer);
        }
        // fallback to heap if buffer is not available or too small
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n) noexcept {
        if (reinterpret_cast<void*>(p) == reinterpret_cast<void*>(&m_buffer)) {
            m_used = false;
        } else {
            ::operator delete(p);
        }
    }

    template <typename U>
    struct rebind {
        using other = TSizedInlineAllocator<U, NumInlineElements, IndexSize>;
    };

    // Allocators are equal only if they're the same object (for stack buffer)
    bool operator==(const TSizedInlineAllocator& other) const noexcept { return this == &other; }
    bool operator!=(const TSizedInlineAllocator& other) const noexcept { return !(*this == other); }

private:
    alignas(T) std::byte m_buffer[sizeof(T) * NumInlineElements];
    bool m_used;
};

// helper alias
template <typename T, int NumInlineElements>
using TInlineAllocator = TSizedInlineAllocator<T, NumInlineElements, 32>;

template <typename T, int NumInlineElements>
using TInlineAllocator64 = TSizedInlineAllocator<T, NumInlineElements, 64>;