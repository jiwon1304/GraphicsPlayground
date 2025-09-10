template <typename T, int NumInlineElements, int IndexSize>
struct TSizedInlineAllocator {
    using SizeType = typename TBitsToSizeType<IndexSize>::Type;
    using value_type = T;
    using size_type = std::make_unsigned_t<SizeType>;
    using difference_type = std::make_signed_t<SizeType>;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    template <typename U>
    struct rebind {
        using other = TSizedInlineAllocator<U, NumInlineElements, IndexSize>;
    };

    TSizedInlineAllocator() noexcept = default;
    ~TSizedInlineAllocator() = default;

    T* allocate(size_type n) noexcept {
        assert( n < NumInlineElements);
        return reinterpret_cast<T*>(InlineBuffer);
    }

    void deallocate(T* p, size_type n) noexcept
    {
        assert( p == reinterpret_cast<T*>(InlineBuffer) );
    }

    // We does not need construct() / destroy()

    size_type max_size() const
    {
        return NumInlineElements;
    }

    /** allocated in the stack memory */
    alignas(T) char InlineBuffer[sizeof(T) * NumInlineElements];
};

template <typename T, int NumInlineElements>
using TInlineAllocator = TSizedInlineAllocator<T, NumInlineElements, 32>;

template <typename T, int NumInlineElements>
using TInlineAllocator64 = TSizedInlineAllocator<T, NumInlineElements, 64>;
