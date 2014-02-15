#pragma once

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
    typedef typename RemoveConstVolatileReference<typename RemoveAllExtents<T>::type>::type ElementType;

    template<class U, ENABLE_IF(!IsArray<U>::value && 0 == Extent<U>::value), class... Args>
    friend UniquePtr<U> MakeUnique(Args&&... args);

    template<class U, ENABLE_IF(IsArray<U>::value && 0 == Extent<U>::value)>
    friend UniquePtr<U> MakeUnique(size_t size, bool is_default_initialized);

public:
    DELETE_COPY(UniquePtr);

    UniquePtr(UniquePtr&& other)
        : _p(other._p), _max_size(other._max_size), _valid_size(other._valid_size)
    {
        other._p = nullptr;
        other._max_size = 0;
        other._valid_size = 0;
    }

    DEFINE_MOVE_ASSIGNER(UniquePtr);
    void swap(UniquePtr& other)
    {
        if (this != &other)
        {
            Swap(_p, other._p);
            Swap(_max_size, other._max_size);
            Swap(_valid_size, other._valid_size);
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
        : _p(), _max_size(), _valid_size()
    {}

    UniquePtr duplicate() const
    {
        if (!_p)
        {
            return {};
        }

        auto tmp = MakeUnique<T>(_max_size);
        FOR (i, _valid_size)
        {
            tmp._p[i] = _p[i];
        }

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

    size_t get_max_size() const
    {
        return _max_size;
    }

    size_t get_valid_size() const
    {
        return _valid_size;
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
        auto tmp = _p;

        _p = nullptr;
        _max_size = 0;
        _valid_size = 0;

        return tmp;
    }

    bool set_valid_size(size_t new_valid_size)
    {
        if (new_valid_size <= _max_size)
        {
            _valid_size = new_valid_size;

            return true;
        }

        return false;
    }

private:
    explicit UniquePtr(ElementType* p, size_t max_size, size_t valid_size)
        : _p(p), _max_size(max_size), _valid_size(valid_size)
    {}

private:
    ElementType* _p;
    size_t       _max_size;
    size_t       _valid_size;
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