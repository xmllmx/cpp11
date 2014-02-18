#include <ntnative/zw_user_mode.hpp>
#include <ntnative/zw_file.hpp>

#define DEFINE_FUNCTION_POINTER(function_name) function_name##Routine function_name##U = 0

DEFINE_FUNCTION_POINTER(RtlInitUnicodeString);
DEFINE_FUNCTION_POINTER(ZwCreateFile);
DEFINE_FUNCTION_POINTER(ZwClose);
DEFINE_FUNCTION_POINTER(ZwQueryInformationFile);
DEFINE_FUNCTION_POINTER(ZwSetInformationFile);
DEFINE_FUNCTION_POINTER(ZwQueryObject);
DEFINE_FUNCTION_POINTER(ZwDeviceIoControlFile);
DEFINE_FUNCTION_POINTER(ZwSetEaFile);
DEFINE_FUNCTION_POINTER(ZwQueryEaFile);
DEFINE_FUNCTION_POINTER(ZwQueryDirectoryFile);
DEFINE_FUNCTION_POINTER(ZwReadFile);
DEFINE_FUNCTION_POINTER(ZwWriteFile);
DEFINE_FUNCTION_POINTER(ZwOpenSymbolicLinkObject);
DEFINE_FUNCTION_POINTER(ZwQuerySymbolicLinkObject);
DEFINE_FUNCTION_POINTER(ZwQueryInformationProcess);
DEFINE_FUNCTION_POINTER(ZwQueryDirectoryObject);
DEFINE_FUNCTION_POINTER(ZwOpenDirectoryObject);
DEFINE_FUNCTION_POINTER(ZwSetSecurityObject);
DEFINE_FUNCTION_POINTER(ZwQuerySecurityObject);
DEFINE_FUNCTION_POINTER(ZwFsControlFile);
DEFINE_FUNCTION_POINTER(ZwOpenProcessTokenEx);
DEFINE_FUNCTION_POINTER(ZwAdjustPrivilegesToken);
DEFINE_FUNCTION_POINTER(ZwQueryVolumeInformationFile);
DEFINE_FUNCTION_POINTER(ZwQueryFullAttributesFile);
DEFINE_FUNCTION_POINTER(ZwOpenProcess);
DEFINE_FUNCTION_POINTER(ZwTerminateProcess);

DEFINE_FUNCTION_POINTER(RtlCreateSecurityDescriptor);
DEFINE_FUNCTION_POINTER(RtlSetDaclSecurityDescriptor);
DEFINE_FUNCTION_POINTER(RtlGetVersion);
DEFINE_FUNCTION_POINTER(RtlRandomEx);

using FARPROC = INT_PTR (__stdcall*)(void);

extern "C" FARPROC __stdcall GetProcAddress(HANDLE hModule, LPCSTR lpProcName);
extern "C" HANDLE  __stdcall LoadLibraryW(LPCWSTR lpLibFileName);

ANS_BEGIN();
struct ZwInitializer
{
    ZwInitializer();
} _UID_;

ZwInitializer::ZwInitializer()
{
	auto hlib = LoadLibraryW(L"ntdll.dll");

#define LOAD_ADDRESS(function_name) \
    function_name##U = reinterpret_cast<function_name##Routine>(GetProcAddress(hlib, #function_name))

	LOAD_ADDRESS(RtlInitUnicodeString);
	LOAD_ADDRESS(ZwCreateFile);
	LOAD_ADDRESS(ZwClose);
	LOAD_ADDRESS(ZwQueryInformationFile);
	LOAD_ADDRESS(ZwSetInformationFile);
	LOAD_ADDRESS(ZwQueryObject);
	LOAD_ADDRESS(ZwDeviceIoControlFile);
	LOAD_ADDRESS(ZwSetEaFile);
	LOAD_ADDRESS(ZwQueryEaFile);
	LOAD_ADDRESS(ZwQueryDirectoryFile);
	LOAD_ADDRESS(ZwReadFile);
	LOAD_ADDRESS(ZwWriteFile);
    LOAD_ADDRESS(ZwOpenSymbolicLinkObject);
    LOAD_ADDRESS(ZwQuerySymbolicLinkObject);
    LOAD_ADDRESS(ZwQueryInformationProcess);
    LOAD_ADDRESS(ZwQueryDirectoryObject);
    LOAD_ADDRESS(ZwOpenDirectoryObject);
    LOAD_ADDRESS(ZwSetSecurityObject);
    LOAD_ADDRESS(ZwQuerySecurityObject);
    LOAD_ADDRESS(ZwFsControlFile);
    LOAD_ADDRESS(ZwOpenProcessTokenEx);
    LOAD_ADDRESS(ZwAdjustPrivilegesToken);
    LOAD_ADDRESS(ZwQueryVolumeInformationFile);
    LOAD_ADDRESS(ZwQueryFullAttributesFile);
    LOAD_ADDRESS(ZwOpenProcess);
    LOAD_ADDRESS(ZwTerminateProcess);

    LOAD_ADDRESS(RtlCreateSecurityDescriptor);
    LOAD_ADDRESS(RtlSetDaclSecurityDescriptor);
    LOAD_ADDRESS(RtlGetVersion);
    LOAD_ADDRESS(RtlRandomEx);
}
ANS_END();