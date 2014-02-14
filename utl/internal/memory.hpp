#pragma once

class Pointer
{
public:
    Pointer()
        : _p(nullptr)
    {}

    Pointer(const void* p)
        : _p(p)
    {}

    const void* Get() const
    {
        return _p;
    }

    template<class Dest, ENABLE_IF(IsPointer<Dest>::value)>
    Dest Cast() const
    {
        return static_cast<Dest>(const_cast<void*>(_p));
    }

private:
    const void* _p;
};

template<class Pointer = void*>
Pointer Allocate(size_t size_in_bytes)
{
    return Pointer(operator new [](size_in_bytes));
}

inline void Free(void* p)
{
    operator delete [](p);
}

template<class T>
struct Deleter final
{
    void operator ()(T* p) const
    {
        delete p;
    }
};

template<class T>
struct Deleter<T[]> final
{
    void operator ()(T* p) const
    {
        delete[] p;
    }
};

struct RawDeleter final
{
    void operator ()(void* p) const
    {
        Free(p);
    }
};

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

INTERFACE IChunker
{
	virtual ~IChunker() = default;

	size_t GetChunkSize() const
    {
        return _chunk_size;
    }

	virtual void* Allocate() = 0;
	virtual void  Deallocate(void* chunk) = 0;

protected:
	IChunker(size_t chunk_size = 0)
		: _chunk_size(chunk_size)
	{}
	
private:
	size_t _chunk_size;
};

template<size_t t_chunk_size>
struct TrivialChunker final : IChunker
{
	TrivialChunker()
		: IChunker(t_chunk_size)
	{}

    void* Allocate() override
    {
        return ::Allocate(t_chunk_size);
    }

    void Deallocate(void* chunk) override
    {
        ::Free(chunk);
    }
};

template<class T>
class UniquePtr final
{
public:
    DELETE_COPY(UniquePtr);

    typedef typename RemoveConstVolatileReference<typename RemoveAllExtents<T>::type>::type ElementType;

    UniquePtr()
        : _p()
    {}

    explicit UniquePtr(ElementType* p)
        : _p(p)
    {}

    ~UniquePtr()
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

    UniquePtr(UniquePtr&& other)
        : _p(other._p)
    {
        other._p = nullptr;
    }

    UniquePtr& operator =(UniquePtr&& other)
    {
        if (this != &other)
        {
            _p = other._p;
        }

        return *this;
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

    explicit operator bool() const noexcept
    {
        return !!_p;
    }

    void swap(UniquePtr& other) noexcept
    {
        ::Swap(_p, other._p);
    }

    ElementType* get() noexcept
    {
        return _p;
    }

    const ElementType* get() const noexcept
    {
        return _p;
    }

    ElementType* release() noexcept
    {
        auto tmp = _p;
        _p = nullptr;

        return tmp;
    }

    void reset(ElementType* p) noexcept
    {
        Destruct<UniquePtr>(this);
        _p = p;
    }

private:
    ElementType* _p;
};

template<class T, class... Args, ENABLE_IF(!IsArray<T>::value && 0 == Extent<T>::value)>
UniquePtr<T> MakeUnique(Args&&... args)
{
    return UniquePtr<T>(new T(Forward<Args...>(args...)));
}

template<class T, ENABLE_IF(IsArray<T>::value && 0 == Extent<T>::value)>
UniquePtr<T> MakeUnique(size_t size)
{
    typedef typename RemoveConstVolatileReference<RemoveAllExtents<T>::type>::type ElementType;

    return UniquePtr<T>(new ElementType[size]());
}

template<class T, class... Args>
typename EnableIf<Extent<T>::value != 0>::type MakeUnique(Args&&...) = delete;

template<class T>
class BasicBuffer final
{
public:
    BasicBuffer()
        : _buf_size(), _buf()
    {}

    BasicBuffer(size_t buf_size)
        : _buf_size(buf_size), _buf(MakeUnique<T[]>(buf_size))
    {}

    BasicBuffer(const BasicBuffer& other)
        : BasicBuffer(other.size())
    {
        FOR (i, _buf_size)
        {
            _buf[i] = other._buf[i];
        }
    }

    BasicBuffer(BasicBuffer&& other)
        : _buf(Move(other._buf))
    {}

    void swap(BasicBuffer& other) noexcept
    {
        ::Swap(_buf, other._buf);
    }

    DEFINE_COPY_ASSIGNER(BasicBuffer);
    DEFINE_MOVE_ASSIGNER(BasicBuffer);

    explicit operator bool() const
    {
        return !!_buf_size;
    }

    size_t size() const
    {
        return _buf_size;
    }

    const T& operator [](size_t idx) const
    {
        return _buf[idx];
    }

    T& operator [](size_t idx)
    {
        return _buf[idx];
    }

    template<class DestPointer = const void*>
    DestPointer get() const
    {
        return reinterpret_cast<DestPointer>(_buf.get());
    }

    template<class DestPointer = void*>
    DestPointer get()
    {
        return reinterpret_cast<DestPointer>(_buf.get());
    }

    operator const T*() const
    {
        return _buf.get();
    }

    operator T*()
    {
        return _buf.get();
    }

    void zero()
    {
        FOR(i, _buf_size)
        {
            _buf[i] = 0;
        }
    }

    void clear()
    {
        _buf_size = 0;
        _buf.release();        
    }

private:
    size_t         _buf_size;
    UniquePtr<T[]> _buf;    
};

typedef BasicBuffer<Byte> Buffer;