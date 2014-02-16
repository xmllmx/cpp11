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
/**/ULONG PrintEx(PCHAR format, ...);
#endif

#include <internal/log.hpp>

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

class ObjectAttributes final
{
private:
    ObjectAttributes(AutoInitFlag)
        : _file_id(), _name(), _oa()
    {}

public:
    ObjectAttributes()
        : ObjectAttributes(auto_init)
    {
        InitializeObjectAttributes(&_oa, nullptr, OBJ_KERNEL_HANDLE|OBJ_CASE_INSENSITIVE, nullptr, nullptr);
    }

    ObjectAttributes(const UNICODE_STRING* name, HANDLE root_dir = 0, ULONG attributes = OBJ_KERNEL_HANDLE|OBJ_CASE_INSENSITIVE, PSECURITY_DESCRIPTOR sec_info = 0)
        : _file_id(), _name(name), _oa()
    {	
        InitializeObjectAttributes(&_oa, _name, attributes, root_dir, sec_info);
    }
    
    ObjectAttributes(HANDLE root_dir, uint64_t file_id, ULONG attributes = OBJ_KERNEL_HANDLE|OBJ_CASE_INSENSITIVE, PSECURITY_DESCRIPTOR sec_info = 0)
        : _file_id(file_id), _name(reinterpret_cast<const wchar_t*>(&_file_id), sizeof(_file_id) / sizeof(wchar_t), false), _oa()
    {
        InitializeObjectAttributes(&_oa, _name, attributes, root_dir, sec_info);
    }

    ObjectAttributes(const wchar_t* name, HANDLE root_dir = 0, ULONG attributes = OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, PSECURITY_DESCRIPTOR sec_info = 0)
        : _file_id(), _name(name), _oa()
    {	
        InitializeObjectAttributes(&_oa, _name, attributes, root_dir, sec_info);
    }

    ObjectAttributes(const wchar_t* sz_name, size_t sz_name_len, HANDLE root_dir = 0, ULONG attributes = OBJ_KERNEL_HANDLE|OBJ_CASE_INSENSITIVE, PSECURITY_DESCRIPTOR sec_info = 0)
        : _file_id(), _name(sz_name, sz_name_len), _oa()
    {
        InitializeObjectAttributes(&_oa, _name, attributes, root_dir, sec_info);
    }

    operator POBJECT_ATTRIBUTES() const
    {
        return &const_cast<ObjectAttributes*>(this)->_oa;
    }

    const UNICODE_STRING* GetName() const
    {
        return _name;
    }

private:
    uint64_t          _file_id;
    UnicodeString     _name;
    OBJECT_ATTRIBUTES _oa;
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

class EaInfo final
{
public:
    UCHAR      flags;
    AnsiString name;
    Buffer value;

public:
    DISABLE_COPY(EaInfo);

    EaInfo()
        : flags(), name(), value()
    {}

    EaInfo(EaInfo&& other)
        : flags(other.flags), name(Move(other.name)), value(Move(other.value))
    {
        other.flags = {};
    }

    DEFINE_MOVE_ASSIGNER(EaInfo);
    DEFINE_SWAP_3(EaInfo, flags, name, value);
};

class StreamInfo final
{
public:
    LargeInteger   size;
    LargeInteger   allocation_size;
    WideString name;

public:
    DISABLE_COPY(StreamInfo);

    StreamInfo()
        : size(), allocation_size(), name()
    {}

    StreamInfo(StreamInfo&& other)
        : size(other.size), allocation_size(other.allocation_size), name(Move(other.name))
    {
        other.size            = 0;
        other.allocation_size = 0;
    }

    DEFINE_MOVE_ASSIGNER(StreamInfo);
    DEFINE_SWAP_3(StreamInfo, size, allocation_size, name);
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

    DEFINE_SWAP_1(NtLastStatus, _status);
};

inline Buffer QueryNtData(Function<NTSTATUS(Buffer&, ULONG*)> fn_query,
                          NTSTATUS* ret_status,
                          size_t init_buf_size = PAGE_SIZE,
                          Function<bool(NTSTATUS)> fn_should_terminate = 
                          [](NTSTATUS status) { return NT_SUCCESS(status); })
{
    Buffer buf;

    if (init_buf_size < PAGE_SIZE)
    {
        buf = MakeBuffer(PAGE_SIZE);
    }
    else
    {
        buf = MakeBuffer(init_buf_size);
    }

    buf.ValidSize(0);

    auto& status = *ret_status;

    while (true)
    {
        ULONG ret_size = 0;

        status = fn_query(buf, &ret_size);

        if (fn_should_terminate(status))
        {
            Assert(ret_size <= buf.MaxSize());
            buf.ValidSize(ret_size);

            return buf;
        }

        if (NT_SUCCESS(status) || STATUS_INSUFFICIENT_RESOURCES == status ||
            STATUS_BUFFER_TOO_SMALL == status || STATUS_BUFFER_OVERFLOW == status)
        {
            buf.MaxSize(buf.MaxSize() * 2);

            continue;
        }
        
        break;
    }

    return {};
}