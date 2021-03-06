#include <ntkrnl/kcpp.hpp>

void* Allocate(size_t cb_size, uint64_t options)
{
    if (0 == options)
    {
        options = NonPagedPool;
    }

    return ExAllocatePoolWithTag(POOL_TYPE(options), cb_size, KNEW_POOL_TAG);
}

void Free(void* p)
{
    ExFreePoolWithTag(p, KNEW_POOL_TAG);
}

void PrintEx(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vDbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, format, args);
    va_end(args);
}

EndLine EndL;
Driver* g_drv = nullptr;

ZwQueryInformationProcessRoutine ZwQueryInformationProcess = nullptr;
ZwAdjustPrivilegesTokenRoutine   ZwAdjustPrivilegesToken   = nullptr;
ZwQueryDirectoryObjectRoutine    ZwQueryDirectoryObject    = nullptr;

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path)
{
    UNICODE_STRING routine_name = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
    ZwQueryInformationProcess   = static_cast<ZwQueryInformationProcessRoutine>(MmGetSystemRoutineAddress(&routine_name));
    Assert(ZwQueryInformationProcess);

    routine_name = RTL_CONSTANT_STRING(L"ZwAdjustPrivilegesToken");
    ZwAdjustPrivilegesToken = static_cast<ZwAdjustPrivilegesTokenRoutine>(MmGetSystemRoutineAddress(&routine_name));
    Assert(ZwAdjustPrivilegesToken);
        
    routine_name = RTL_CONSTANT_STRING(L"ZwQueryDirectoryObject");
    ZwQueryDirectoryObject = static_cast<ZwQueryDirectoryObjectRoutine>(MmGetSystemRoutineAddress(&routine_name));
    Assert(ZwQueryDirectoryObject);

    auto drv_instance = CreateDriverInstance(drv_obj, reg_path);

    Assert(g_drv);
    Assert(g_drv == drv_instance);

    auto status = g_drv->Initialize();
    if (!NT_SUCCESS(status))
    {
        SAFE_DELETE(g_drv);
    }

    return status;
}

NTSTATUS Driver::Initialize()
{
    return STATUS_SUCCESS;
}

PDRIVER_OBJECT Driver::Get() const
{
    Assert(_drv_obj);

    return _drv_obj;
}

VOID Driver::_OnUnloadDriver()
{}

Driver::Driver(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path, bool is_unloadable)
    : _drv_obj(drv_obj)
{
    Assert(nullptr == g_drv);

    g_drv = this;

    if (is_unloadable)
    {
        _drv_obj->DriverUnload = Driver::_UnloadDriver;
    }
}

VOID Driver::_UnloadDriver(PDRIVER_OBJECT drv_obj)
{
    g_drv->_OnUnloadDriver();
    SAFE_DELETE(g_drv);
}