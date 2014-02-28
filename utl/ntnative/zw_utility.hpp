#pragma once

template<class T, ENABLE_IF(IsPointer<T>::value)>
class Unused final
{
public:
    operator T()
    {
        return &_dummy;
    }

private:
    typename RemovePointer<T>::type _dummy;
};

#define DECLARE_FUNCTION_POINTER(function_name) extern function_name##Routine function_name##U

typedef struct _OBJECT_DIRECTORY_INFORMATION {
    UNICODE_STRING Name;
    UNICODE_STRING TypeName;
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

using RtlInitUnicodeStringRoutine = VOID(__stdcall*)(PUNICODE_STRING DestinationString, 
                                                     PCWSTR SourceString);

using ZwQueryInformationFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle, 
                                                           PIO_STATUS_BLOCK IoStatusBlock,
                                                           PVOID FileInformation, 
                                                           ULONG Length, 
                                                           FILE_INFORMATION_CLASS FileInformationClass);

using ZwSetInformationFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle, 
                                                         PIO_STATUS_BLOCK IoStatusBlock, 
                                                         PVOID FileInformation, 
                                                         ULONG Length,
                                                         FILE_INFORMATION_CLASS FileInformationClass);

using ZwCreateFileRoutine = NTSTATUS(__stdcall*)(PHANDLE FileHandle,
                                                 ACCESS_MASK DesiredAccess,
                                                 POBJECT_ATTRIBUTES ObjectAttributes,
                                                 PIO_STATUS_BLOCK IoStatusBlock,
                                                 PLARGE_INTEGER AllocationSize, 
                                                 ULONG FileAttributes,
                                                 ULONG ShareAccess, 
                                                 ULONG CreateDisposition,
                                                 ULONG CreateOptions, 
                                                 PVOID EaBuffer, 
                                                 ULONG EaLength);

using ZwQueryObjectRoutine = NTSTATUS(__stdcall*)(HANDLE Handle, 
                                                  OBJECT_INFORMATION_CLASS ObjectInformationClass,
                                                  PVOID ObjectInformation,
                                                  ULONG ObjectInformationLength, 
                                                  PULONG ReturnLength);

using ZwCloseRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle);

using ZwDeviceIoControlFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle, 
                                                          HANDLE Event, 
                                                          PIO_APC_ROUTINE ApcRoutine,
                                                          PVOID ApcContext,
                                                          PIO_STATUS_BLOCK IoStatusBlock,
                                                          ULONG IoControlCode,
                                                          PVOID InputBuffer,
                                                          ULONG InputBufferLength, 
                                                          PVOID OutputBuffer,
                                                          ULONG OutputBufferLength);

using ZwSetEaFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle,
                                                PIO_STATUS_BLOCK IoStatusBlock,
                                                PVOID Buffer,
                                                ULONG Length);

using ZwQueryEaFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle,
                                                  PIO_STATUS_BLOCK IoStatusBlock,
                                                  PVOID Buffer,
                                                  ULONG Length,
                                                  BOOLEAN ReturnSingleEntry,
                                                  PVOID EaList,
                                                  ULONG EaListLength,
                                                  PULONG EaIndex,
                                                  BOOLEAN RestartScan);

using ZwQueryDirectoryFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle,
                                                         HANDLE Event,
                                                         PIO_APC_ROUTINE ApcRoutine,
                                                         PVOID ApcContext,
                                                         PIO_STATUS_BLOCK IoStatusBlock,
                                                         PVOID FileInformation, 
                                                         ULONG Length, 
                                                         FILE_INFORMATION_CLASS FileInformationClass,
                                                         BOOLEAN ReturnSingleEntry,
                                                         PUNICODE_STRING FileName,
                                                         BOOLEAN RestartScan);

using ZwReadFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle,
                                               HANDLE Event,
                                               PIO_APC_ROUTINE ApcRoutine,
                                               PVOID ApcContext,
                                               PIO_STATUS_BLOCK IoStatusBlock,
                                               PVOID Buffer,
                                               ULONG Length,
                                               PLARGE_INTEGER ByteOffset,
                                               PULONG Key);

using ZwWriteFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle, 
                                                HANDLE Event, 
                                                PIO_APC_ROUTINE ApcRoutine, 
                                                PVOID ApcContext, 
                                                PIO_STATUS_BLOCK IoStatusBlock,
                                                PVOID Buffer,
                                                ULONG Length,
                                                PLARGE_INTEGER ByteOffset, 
                                                PULONG Key);

using ZwOpenSymbolicLinkObjectRoutine = NTSTATUS(__stdcall*)(PHANDLE LinkHandle,
                                                             ACCESS_MASK DesiredAccess,
                                                             POBJECT_ATTRIBUTES ObjectAttributes);

using ZwQuerySymbolicLinkObjectRoutine = NTSTATUS(__stdcall*)(HANDLE LinkHandle, 
                                                              PUNICODE_STRING LinkTarget,
                                                              PULONG ReturnedLength);

using ZwQueryInformationProcessRoutine = NTSTATUS(__stdcall*)(HANDLE ProcessHandle,
                                                              PROCESSINFOCLASS ProcessInformationClass,
                                                              PVOID ProcessInformation,
                                                              ULONG ProcessInformationLength,
                                                              PULONG ReturnLength);

using ZwOpenDirectoryObjectRoutine = NTSTATUS(__stdcall*)(OUT PHANDLE DirectoryHandle, 
                                                          IN ACCESS_MASK DesiredAccess,
                                                          IN POBJECT_ATTRIBUTES ObjectAttributes);

using ZwQueryDirectoryObjectRoutine = NTSTATUS(__stdcall*)(IN HANDLE DirectoryHandle,
                                                           OUT OPTIONAL PVOID Buffer,
                                                           ULONG Length, 
                                                           BOOLEAN ReturnSingleEntry,
                                                           BOOLEAN RestartScan,
                                                           IN OUT PULONG Context,
                                                           OUT OPTIONAL PULONG ReturnLength);

using ZwSetSecurityObjectRoutine = NTSTATUS(__stdcall*)(HANDLE Handle,
                                                        SECURITY_INFORMATION SecurityInformation,
                                                        PSECURITY_DESCRIPTOR SecurityDescriptor);

using ZwQuerySecurityObjectRoutine = NTSTATUS(__stdcall*)(HANDLE Handle, 
                                                          SECURITY_INFORMATION SecurityInformation,
                                                          PSECURITY_DESCRIPTOR SecurityDescriptor,
                                                          ULONG Length,
                                                          PULONG LengthNeeded);

using ZwFsControlFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle,
                                                    HANDLE Event,
                                                    PIO_APC_ROUTINE ApcRoutine,
                                                    PVOID ApcContext,
                                                    PIO_STATUS_BLOCK IoStatusBlock,
                                                    ULONG FsControlCode,
                                                    PVOID InputBuffer,
                                                    ULONG InputBufferLength,
                                                    PVOID OutputBuffer,
                                                    ULONG OutputBufferLength);

using ZwOpenProcessTokenExRoutine = NTSTATUS(__stdcall*)(HANDLE ProcessHandle,
                                                         ACCESS_MASK DesiredAccess,
                                                         ULONG HandleAttributes,
                                                         PHANDLE TokenHandle);

using ZwAdjustPrivilegesTokenRoutine = NTSTATUS(__stdcall*)(HANDLE TokenHandle,
                                                            BOOLEAN DisableAllPrivileges,
                                                            PTOKEN_PRIVILEGES NewState, 
                                                            ULONG BufferLength,
                                                            PTOKEN_PRIVILEGES PreviousState, 
                                                            PULONG ReturnLength);

using ZwQueryVolumeInformationFileRoutine = NTSTATUS(__stdcall*)(HANDLE FileHandle,
                                                                 PIO_STATUS_BLOCK IoStatusBlock,
                                                                 PVOID FsInformation,
                                                                 ULONG Length, 
                                                                 FS_INFORMATION_CLASS FsInformationClass);

using ZwQueryFullAttributesFileRoutine = NTSTATUS(__stdcall*)(IN POBJECT_ATTRIBUTES ObjectAttributes,
                                                              OUT PFILE_NETWORK_OPEN_INFORMATION FileInformation);

using ZwOpenProcessRoutine = NTSTATUS(__stdcall*)(PHANDLE ProcessHandle, 
                                                  ACCESS_MASK DesiredAccess,
                                                  POBJECT_ATTRIBUTES ObjectAttributes,
                                                  PCLIENT_ID ClientId);

using ZwTerminateProcessRoutine = NTSTATUS(__stdcall*)(IN HANDLE ProcessHandle,
                                                       IN NTSTATUS ExitStatus);

using RtlCreateSecurityDescriptorRoutine = NTSTATUS(__stdcall*)(PSECURITY_DESCRIPTOR sd, 
                                                                ULONG revision); // revision must be SECURITY_DESCRIPTOR_REVISION

using RtlSetDaclSecurityDescriptorRoutine = NTSTATUS(__stdcall*)(PSECURITY_DESCRIPTOR sd,
                                                                 BOOLEAN is_dacl_present,
                                                                 PACL dacl,
                                                                 BOOLEAN default_dacl);

using RtlGetVersionRoutine = NTSTATUS(__stdcall*)(IN OUT PRTL_OSVERSIONINFOW lpVersionInformation);

using RtlRandomExRoutine = ULONG(__stdcall*)(IN OUT PULONG seed);

DECLARE_FUNCTION_POINTER(RtlInitUnicodeString);
DECLARE_FUNCTION_POINTER(ZwCreateFile);
DECLARE_FUNCTION_POINTER(ZwClose);
DECLARE_FUNCTION_POINTER(ZwQueryInformationFile);
DECLARE_FUNCTION_POINTER(ZwSetInformationFile);
DECLARE_FUNCTION_POINTER(ZwQueryObject);
DECLARE_FUNCTION_POINTER(ZwDeviceIoControlFile);
DECLARE_FUNCTION_POINTER(ZwSetEaFile);
DECLARE_FUNCTION_POINTER(ZwQueryEaFile);
DECLARE_FUNCTION_POINTER(ZwQueryDirectoryFile);
DECLARE_FUNCTION_POINTER(ZwReadFile);
DECLARE_FUNCTION_POINTER(ZwWriteFile);
DECLARE_FUNCTION_POINTER(ZwOpenSymbolicLinkObject);
DECLARE_FUNCTION_POINTER(ZwQuerySymbolicLinkObject);
DECLARE_FUNCTION_POINTER(ZwQueryInformationProcess);
DECLARE_FUNCTION_POINTER(ZwQueryDirectoryObject);
DECLARE_FUNCTION_POINTER(ZwOpenDirectoryObject);
DECLARE_FUNCTION_POINTER(ZwSetSecurityObject);
DECLARE_FUNCTION_POINTER(ZwQuerySecurityObject);
DECLARE_FUNCTION_POINTER(ZwFsControlFile);
DECLARE_FUNCTION_POINTER(ZwOpenProcessTokenEx);
DECLARE_FUNCTION_POINTER(ZwAdjustPrivilegesToken);
DECLARE_FUNCTION_POINTER(ZwQueryVolumeInformationFile);
DECLARE_FUNCTION_POINTER(ZwQueryFullAttributesFile);
DECLARE_FUNCTION_POINTER(ZwOpenProcess);
DECLARE_FUNCTION_POINTER(ZwTerminateProcess);

DECLARE_FUNCTION_POINTER(RtlCreateSecurityDescriptor);
DECLARE_FUNCTION_POINTER(RtlSetDaclSecurityDescriptor);
DECLARE_FUNCTION_POINTER(RtlGetVersion);
DECLARE_FUNCTION_POINTER(RtlRandomEx);

#include <ntnative/ntstring.hpp>

#if defined(ZW_USER_MODE_)
#define RtlInitUnicodeString RtlInitUnicodeStringU
#define ZwCreateFile ZwCreateFileU
#define ZwClose ZwCloseU
#define ZwQueryInformationFile ZwQueryInformationFileU
#define ZwSetInformationFile ZwSetInformationFileU
#define ZwQueryObject ZwQueryObjectU
#define ZwDeviceIoControlFile ZwDeviceIoControlFileU
#define ZwSetEaFile ZwSetEaFileU
#define ZwQueryEaFile ZwQueryEaFileU
#define ZwQueryDirectoryFile ZwQueryDirectoryFileU
#define ZwReadFile ZwReadFileU
#define ZwWriteFile ZwWriteFileU
#define ZwOpenSymbolicLinkObject ZwOpenSymbolicLinkObjectU
#define ZwQuerySymbolicLinkObject ZwQuerySymbolicLinkObjectU
#define ZwQueryInformationProcess ZwQueryInformationProcessU
#define ZwQueryDirectoryObject ZwQueryDirectoryObjectU
#define ZwOpenDirectoryObject ZwOpenDirectoryObjectU
#define ZwSetSecurityObject ZwSetSecurityObjectU
#define ZwQuerySecurityObject ZwQuerySecurityObjectU
#define ZwFsControlFile ZwFsControlFileU
#define ZwQueryInformationProcess ZwQueryInformationProcessU
#define ZwOpenProcessTokenEx ZwOpenProcessTokenExU
#define ZwAdjustPrivilegesToken ZwAdjustPrivilegesTokenU
#define ZwQueryVolumeInformationFile ZwQueryVolumeInformationFileU
#define ZwQueryFullAttributesFile ZwQueryFullAttributesFileU
#define ZwOpenProcess ZwOpenProcessU
#define ZwTerminateProcess ZwTerminateProcessU

#define RtlCreateSecurityDescriptor RtlCreateSecurityDescriptorU
#define RtlSetDaclSecurityDescriptor RtlSetDaclSecurityDescriptorU
#define RtlGetVersion RtlGetVersionU
#define RtlRandomEx RtlRandomExU
#else
extern ZwQueryInformationProcessRoutine ZwQueryInformationProcess;
extern ZwAdjustPrivilegesTokenRoutine   ZwAdjustPrivilegesToken;
extern ZwQueryDirectoryObjectRoutine    ZwQueryDirectoryObject;
#endif

inline NTSTATUS AdjustPrivilege(bool is_enable, ULONG privilege)
{
    TOKEN_PRIVILEGES new_privs = {};
    HANDLE           h_process_token = {};

    auto status = ZwOpenProcessTokenEx(ZwCurrentProcess(), TOKEN_ALL_ACCESS, OBJ_KERNEL_HANDLE, &h_process_token);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    DEFER(ZwClose(h_process_token));

    new_privs.PrivilegeCount = 1;
    new_privs.Privileges[0].Luid = RtlConvertUlongToLuid(privilege);

    if (is_enable)
    {
        new_privs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        new_privs.Privileges[0].Attributes = 0;
    }

    return ZwAdjustPrivilegesToken(h_process_token, false, &new_privs, sizeof(new_privs), nullptr, nullptr);
}

inline NTSTATUS EnablePrivilege(ULONG privilege)
{
    return AdjustPrivilege(true, privilege);
}

inline NTSTATUS DisablePrivilege(ULONG privilege)
{
    return AdjustPrivilege(false, privilege);
}

inline void EnableAllPrivileges()
{
    for (auto i = SE_MIN_WELL_KNOWN_PRIVILEGE; i <= SE_MAX_WELL_KNOWN_PRIVILEGE; ++i)
    {
        EnablePrivilege(i);
    }
}

class NtMaxPrivilege_ final
{
public:
    NtMaxPrivilege_()
        : _h_process_token(), _previous_state()
    {
        auto status = ZwOpenProcessTokenEx(ZwCurrentProcess(), TOKEN_ALL_ACCESS, OBJ_KERNEL_HANDLE, &_h_process_token);
        Assert(NT_SUCCESS(status) && _h_process_token);

        _previous_state     = MakeBuffer(4096);
        auto previous_state = Cast<PTOKEN_PRIVILEGES>(_previous_state);
        status = ZwAdjustPrivilegesToken(_h_process_token, true, nullptr, _previous_state.capacity(), previous_state, Unused<PULONG>());
        Assert(NT_SUCCESS(status));

        for (ULONG priv_num = SE_MIN_WELL_KNOWN_PRIVILEGE; priv_num <= SE_MAX_WELL_KNOWN_PRIVILEGE; ++priv_num)
        {
            TOKEN_PRIVILEGES new_priv = {};
            new_priv.PrivilegeCount   = 1;

            new_priv.Privileges[0].Luid = RtlConvertUlongToLuid(priv_num);
            new_priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            status = ZwAdjustPrivilegesToken(_h_process_token, false, &new_priv, sizeof(new_priv), nullptr, nullptr);
            Assert(NT_SUCCESS(status));
        }
    }

    ~NtMaxPrivilege_()
    {
        Assert(_h_process_token);

        auto previous_state = Cast<PTOKEN_PRIVILEGES>(_previous_state);

        auto status = ZwAdjustPrivilegesToken(_h_process_token, true, nullptr, 0, nullptr, nullptr);
        Assert(NT_SUCCESS(status));
        status = ZwAdjustPrivilegesToken(_h_process_token, false, previous_state, 0, nullptr, nullptr);
        Assert(NT_SUCCESS(status));
        
        ZwClose(_h_process_token);
    }

private:
    HANDLE _h_process_token;
    Buffer _previous_state;
};

#define ELEVATE_TO_MAX_PRIVILEGES() NtMaxPrivilege_ UID_

//
// e.g. WinXP SP2 : 0x05010200
//
inline ULONG GetOsVersion()
{
    RTL_OSVERSIONINFOW ver_info = {};
    ver_info.dwOSVersionInfoSize = sizeof(ver_info);

    auto status = RtlGetVersion(&ver_info);
    Assert(NT_SUCCESS(status));

    auto ver = ver_info.dwMajorVersion << 24 | ver_info.dwMinorVersion << 16;

    if (0 == ver_info.szCSDVersion[0])
    {
        return ver;
    }

    if (Equals(ver_info.szCSDVersion, L"Service Pack 1", false))
    {
        return ver |= 0x0100;
    }

    if (Equals(ver_info.szCSDVersion, L"Service Pack 2", false))
    {
        return ver |= 0x0200;
    }

    if (Equals(ver_info.szCSDVersion, L"Service Pack 3", false))
    {
        return ver |= 0x0300;
    }

    if (Equals(ver_info.szCSDVersion, L"Service Pack 4", false))
    {
        return ver |= 0x0400;
    }

    if (Equals(ver_info.szCSDVersion, L"Service Pack 5", false))
    {
        return ver |= 0x0500;
    }

    if (Equals(ver_info.szCSDVersion, L"Service Pack 6", false))
    {
        return ver |= 0x0600;
    }

    return ver;
}

template<class CharType>
String<CharType> GenerateRandomString(ULONG seed, size_t str_length)
{
    String<CharType> r_str(str_length);

    FOR(i, str_length)
    {
        CharType c = CharType('A') + CharType(RtlRandomEx(&seed) % 26);
        r_str << c;
    }

    r_str[str_length] = 0;

    return r_str;
}