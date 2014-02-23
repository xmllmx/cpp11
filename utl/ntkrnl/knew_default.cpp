void* __cdecl operator new[](size_t size, POOL_TYPE pool_type)
{
    return ExAllocatePoolWithTag(pool_type, size, KNEW_POOL_TAG);
}

void __cdecl operator delete[](void* p)
{
    ExFreePoolWithTag(p, KNEW_POOL_TAG);
}