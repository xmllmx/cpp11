#pragma once

#define KNEW_POOL_TAG ULONG('wenk')

//
// The following functions are provided by the standard, and they cannot be replaced by the user.
//
//+++
void* __cdecl operator new(size_t size, void* placement) noexcept;
void  __cdecl operator delete(void* p, void* placement) noexcept;

void* __cdecl operator new[](size_t size, void* placement) noexcept;
void  __cdecl operator delete[](void* p, void* placement) noexcept;
//---

void* __cdecl operator new(size_t size);
void  __cdecl operator delete(void* p) noexcept;

void* __cdecl operator new(size_t size, POOL_TYPE pool_type);
void  __cdecl operator delete(void* p, POOL_TYPE pool_type) noexcept;

//
// The following functions can be replaced by the user if the macro 'USE_CUSTOM_NEW_DELETE_' predefined.
//
//+++
void* __cdecl operator new[](size_t size);
void  __cdecl operator delete[](void* p) noexcept;

void* __cdecl operator new[](size_t size, POOL_TYPE pool_type);
void  __cdecl operator delete[](void* p, POOL_TYPE pool_type) noexcept;
//---