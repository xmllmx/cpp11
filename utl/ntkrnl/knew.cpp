#include <ntkrnl/kcpp.hpp>

void __cdecl operator delete(void* p)
{
    ExFreePoolWithTag(p, KNEW_POOL_TAG);
}

void* __cdecl operator new(size_t size)
{
    return ExAllocatePoolWithTag(NonPagedPool, size, KNEW_POOL_TAG);
}

void* __cdecl operator new[](size_t size)
{
    return ExAllocatePoolWithTag(NonPagedPool, size, KNEW_POOL_TAG);
}

void* __cdecl operator new(size_t size, void* place)
{
    return place;
}

void* __cdecl operator new[](size_t size, void* place)
{
    return place;
}

void* __cdecl operator new(size_t size, POOL_TYPE pool_type)
{
    return ExAllocatePoolWithTag(pool_type, size, KNEW_POOL_TAG);
}

#if !defined(USE_CUSTOM_NEW_DELETE_)
#include <ntkrnl/knew_default.cpp>
#endif