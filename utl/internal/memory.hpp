#pragma once

template<class Pointer = void*>
Pointer Allocate(size_t cb_size)
{
    return Pointer(operator new [](cb_size));
}

inline void Free(void* p)
{
    operator delete [](p);
}

template<class T>
void Destruct(T* p)
{
    p->~T();
}

template<class T, ENABLE_IF(!IsPointer<T>::value)>
void Destruct(T& obj)
{
    obj.~T();
}

template<class T>
class UniquePtr final
{
public:
    typedef typename RemoveConstVolatileReference<typename RemoveAllExtents<T>::type>::type ElementType;

    template<class U, ENABLE_IF(!IsArray<U>::value && 0 == Extent<U>::value), class... Args>
    friend UniquePtr<U> MakeUnique(Args&&... args);

    template<class U, ENABLE_IF(IsArray<U>::value && 0 == Extent<U>::value)>
    friend UniquePtr<U> MakeUnique(size_t size, bool is_default_initialized);

public:
    DELETE_COPY(UniquePtr);

    UniquePtr(UniquePtr&& other)
        : _p(other._p), _capacity(other._capacity), _size(other._size)
    {
        other._p        = nullptr;
        other._capacity = 0;
        other._size     = 0;
    }

    DEFINE_MOVE_ASSIGNER(UniquePtr);
    void swap(UniquePtr& other)
    {
        if (this != &other)
        {
            Swap(_p, other._p);
            Swap(_capacity, other._capacity);
            Swap(_size, other._size);
        }
    }

    ~UniquePtr()
    {
        if (_p)
        {
            if (IsArray<T>::value)
            {
                delete[] _p;
            }
            else
            {
                delete _p;
            }
        }

        _p = nullptr;
    }

public:
    UniquePtr()
        : _p(), _capacity(), _size()
    {}

    UniquePtr duplicate() const
    {
        if (!_p)
        {
            return {};
        }

        auto tmp = MakeUnique<T>(_capacity);
        FOR (i, _size)
        {
            tmp._p[i] = _p[i];
        }

        tmp.resize(_size);

        return tmp;
    }

    ElementType* operator ->() const
    {
        return _p;
    }

    ElementType& operator *() const
    {
        return *_p;
    }

    ElementType& operator [](size_t idx) const
    {
        return _p[idx];
    }

    size_t capacity() const
    {
        return _capacity;
    }

    size_t size() const
    {
        return _size;
    }

    explicit operator bool() const
    {
        return !!_p;
    }

    ElementType* get()
    {
        return _p;
    }

    const ElementType* get() const
    {
        return _p;
    }

    ElementType* release()
    {
        auto tmp  = _p;
        _p        = nullptr;
        _capacity = 0;
        _size     = 0;

        return tmp;
    }

    bool resize(size_t new_size)
    {
        if (new_size <= _capacity)
        {
            _size = new_size;

            return true;
        }

        return false;
    }

private:
    explicit UniquePtr(ElementType* p, size_t capacity, size_t size)
        : _p(p), _capacity(capacity), _size(size)
    {}

private:
    ElementType* _p;
    size_t       _capacity;
    size_t       _size;
};

template<class T, ENABLE_IF(!IsArray<T>::value && 0 == Extent<T>::value), class... Args>
UniquePtr<T> MakeUnique(Args&&... args)
{
    return UniquePtr<T>(new T(Forward<Args...>(args...)), 1, 1);
}

template<class T, ENABLE_IF(IsArray<T>::value && 0 == Extent<T>::value)>
UniquePtr<T> MakeUnique(size_t size, bool is_default_initialized = false)
{
    typedef typename RemoveConstVolatileReference<RemoveAllExtents<T>::type>::type ElementType;

    if (0 == size)
    {
        return {};
    }

    if (is_default_initialized)
    {
        return UniquePtr<T>(new ElementType[size](), size, size);
    }
    else
    {
        return UniquePtr<T>(new ElementType[size], size, size);
    }
}

template<class T, class... Args>
typename EnableIf<Extent<T>::value != 0>::type MakeUnique(Args&&...) = delete;

typedef UniquePtr<Byte[]> Buffer;

inline Buffer MakeBuffer(size_t size, bool is_default_initialized = false)
{
    return MakeUnique<Byte[]>(size, is_default_initialized);
}

template<class Dest, ENABLE_IF(IsPointer<Dest>::value)>
Dest Cast(Buffer& buf)
{
    return reinterpret_cast<Dest>(buf.get());
}

template<class Dest, ENABLE_IF(IsPointer<Dest>::value)>
Dest Cast(const Buffer& buf)
{
    return reinterpret_cast<Dest>(buf.get());
}

template<size_t t_block_size>
class Block_ final
{
    static_assert(IsStandardLayout<Block_>::value, "utl error");

public:
    Block_()
    {}

    Block_& operator =(uint8_t n)
    {
        *reinterpret_cast<uint8_t*>(_buf) = n;

        return *this;
    }

    operator uint8_t() const
    {
        return *reinterpret_cast<const uint8_t*>(_buf);
    }

private:
    Byte _buf[t_block_size];
};

template<size_t t_block_size>
class FixedAllocator;

template<size_t t_block_size>
class Chunk_ final
{
    friend class FixedAllocator<t_block_size>;

    typedef Block_<t_block_size> BlockType;
    static constexpr uint8_t sc_block_count = Max<uint8_t>(); // sc_block_count is 255 rather than 256

public:
    void Initialize()
    {
        _buf = MakeUnique<BlockType[]>(sc_block_count);
        _next_free_block_idx = 0;
        _free_blocks_available = sc_block_count;

        FOR(i, sc_block_count)
        {
            _buf[i] = i + 1;
        }
    }

    void* GetBlock()
    {
        if (_free_blocks_available > 0)
        {
            auto result_block = &_buf[_next_free_block_idx];

            _next_free_block_idx = _buf[_next_free_block_idx];
            --_free_blocks_available;

            return result_block;
        }

        return nullptr;
    }

    void FreeBlock(void* p)
    {
        Assert(IsAlignedTo<t_block_size>(PtrDiffInBytes(p - &_buf[0])));

        auto idx = PtrDiffInElements<BlockType>(p - &_buf[0]);
        Assert(idx < sc_block_count);

        _buf[idx] = _next_free_block_idx;
        _next_free_block_idx = idx;
        ++_free_blocks_available;
    }
    
private:
    UniquePtr<BlockType[]> _buf;
    uint8_t                _next_free_block_idx;
    uint8_t                _free_blocks_available;
};

template<size_t t_block_size>
class FixedAllocator
{
    typedef Chunk_<t_block_size> Chunk;

private:
    Chunk _buf[9];
};