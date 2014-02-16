#pragma once

#define Abort() DbgBreakPoint()

#if DBG
#define Print(_x_) Log(_x_)
#else
#define Print(_x_)
#endif

#ifndef K_POOL_TAG
#define K_POOL_TAG 'wenk'
#endif

#define SAFE_CODE()         Assert(PASSIVE_LEVEL == KeGetCurrentIrql())
#define IS_SYSTEM_PROCESS() (PsGetCurrentProcess() == PsInitialSystemProcess)

//
// The macro NDEBUG is not defined, no matter the build environment is checked or free.
//

#if DBG
inline void KernelAssert(bool expr)
{
    if (!expr) 
    {
        DbgBreakPoint();
    }
}
#endif

#if DBG
#define Assert(expr) KernelAssert(!!(expr))
#else
#define Assert(expr) ((void)0)
#endif