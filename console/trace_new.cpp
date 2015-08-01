#include <ucpp.hpp>
#include <iostream>

using namespace std;

uint64_t g_allocated_bytes = 0;

void CheckMemoryLeak()
{
    if (g_allocated_bytes > 0)
    {
        cout << "Memory leak! " << g_allocated_bytes << " bytes." << endl;
    }
}

void* __cdecl operator new(size_t size)
{
    if (auto p = malloc(size + sizeof(uint64_t)))
    {
        g_allocated_bytes += size;
        *static_cast<uint64_t*>(p) = size;

        return MovePtr(p, sizeof(uint64_t));
    }

    return nullptr;
}

void  __cdecl operator delete(void* p) noexcept
{
    auto old_p  = MovePtr(p, - static_cast<int64_t>(sizeof(uint64_t)));
    g_allocated_bytes -= *static_cast<uint64_t*>(old_p);

    free(old_p);
}

void* __cdecl operator new[](size_t size)
{
    return ::operator new(size);
}

void  __cdecl operator delete[](void* p) noexcept
{
    ::operator delete(p);
}