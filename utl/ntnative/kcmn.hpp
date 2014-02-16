#pragma once

#define _NT_NATIVE_MODE_

//
// The following block is just for Visual Studio to correctly recognize some identifiers such as PAGE_SIZE.
// We should use Win32 rather than x64 as the solution platform, or the issue will remain.
// It has no impact on the WDK build process. 
//
#if defined(_MSC_VER) && defined(_M_IX86) && !defined(_X86_)
/**/#define _X86_
#endif

#include <dontuse.h>
#include <suppress.h>
#include <fltKernel.h>

#include <internal/stdcpp.hpp>
#include <ntnative/ntstring.hpp>

#if !defined(_ZW_USER_MODE_)
/**/#define _ZW_KERNEL_MODE_
#endif

#if defined(_ZW_KERNEL_MODE_)
/**/#include <ntkrnl/kmacro.hpp>
/**/#include <ntkrnl/knew.hpp>
/**/#define NTSTRSAFE_LIB
/**/#include <ntstrsafe.h>
#endif

typedef HANDLE ProcessId;
typedef HANDLE ThreadId;

#define RETURN_STATUS_IF_FAILED(status) if (!NT_SUCCESS(status)) { return status; } 0

#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFFF)
#else
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF)
#endif

class UnicodeString final
{
public:
    UnicodeString()
        : _str()
    {}

    UnicodeString(const UNICODE_STRING* str)
        : _str(*str)
    {}

    UnicodeString(const wchar_t* sz)
        : _str()
    {
        _str.Length        = GetLength(sz) * sizeof(wchar_t);
        _str.MaximumLength = _str.Length + sizeof(wchar_t);
        _str.Buffer        = const_cast<wchar_t*>(sz);
    }

    UnicodeString(const wchar_t* sz, size_t sz_len, bool is_null_terminated = true)
        : _str({ USHORT(sz_len * sizeof(wchar_t)), (is_null_terminated ? sz_len + 1 : sz_len) * sizeof(wchar_t), const_cast<wchar_t*>(sz) })
    {}

    template<size_t t_n>
    UnicodeString(const wchar_t (&sz)[t_n])
        : _str({ USHORT(n * sizeof(wchar_t)), (n - 1) * sizeof(wchar_t), const_cast<wchar_t*>(sz) })
    {}

    operator UNICODE_STRING*()
    {
        return &_str;
    }

    operator const UNICODE_STRING*() const
    {
        return &_str;
    }
    
private:
    UNICODE_STRING _str;
};

class LargeInteger final
{
public:
    LargeInteger()
        : _n()
    {}

    LargeInteger(uint64_t n)
        : LargeInteger()
    {
        _n.QuadPart = n;
    }

    LargeInteger(int n)
        : LargeInteger(uint64_t(n))
    {}   

    LargeInteger(LARGE_INTEGER n)
        : LargeInteger(uint64_t(n.QuadPart))
    {}

    LargeInteger(const LARGE_INTEGER* n)
        : LargeInteger(uint64_t(n->QuadPart))
    {}

    operator LARGE_INTEGER()
    {
        return _n;
    }

    operator const LARGE_INTEGER() const
    {
        return _n;
    }

    operator LARGE_INTEGER*()
    {
        return &_n;
    }

    operator const LARGE_INTEGER*() const
    {
        return &_n;
    }

    LargeInteger& operator =(uint64_t n)
    {
        _n.QuadPart = n;

        return *this;
    }

private:
    LARGE_INTEGER _n;
};

class NtLastStatus
{
protected:
    NtLastStatus(NTSTATUS status = STATUS_UNSUCCESSFUL)
        : _status(status)
    {}

    ~NtLastStatus()
    {}

public:
    NTSTATUS GetLastStatus() const
    {
        return _status;
    }

protected:
    mutable NTSTATUS _status;
};