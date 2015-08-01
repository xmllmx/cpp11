void* __cdecl operator new[](size_t size)
{
    return ExAllocatePoolWithTag(NonPagedPool, size, KNEW_POOL_TAG);
}

void __cdecl operator delete[](void* p) noexcept
{
    ExFreePoolWithTag(p, KNEW_POOL_TAG);
}

void* __cdecl operator new[](size_t size, POOL_TYPE pool_type)
{
    return ExAllocatePoolWithTag(pool_type, size, KNEW_POOL_TAG);
}

void __cdecl operator delete[](void* p, POOL_TYPE pool_type) noexcept
{
    ExFreePoolWithTag(p, KNEW_POOL_TAG);
}