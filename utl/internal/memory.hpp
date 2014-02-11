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

interface IChunker
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