#pragma once

#define KNEW_POOL_TAG ULONG('wenk')

void __cdecl operator delete(void* p);
void __cdecl operator delete [](void* p);

void* __cdecl operator new(size_t size);
void* __cdecl operator new [](size_t size);
void* __cdecl operator new(size_t size, void* place);
void* __cdecl operator new [](size_t size, void* place);

void* __cdecl operator new(size_t size, POOL_TYPE pool_type);
void* __cdecl operator new [](size_t size, POOL_TYPE pool_type);