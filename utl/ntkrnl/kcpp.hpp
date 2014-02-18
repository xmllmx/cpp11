#pragma once

#include <ntnative/kcmn.hpp>

#ifndef _ZW_KERNEL_MODE_
/**/#error macro '_ZW_KERNEL_MODE_' not defined
#endif

extern "C" NTSTATUS NTAPI ObReferenceObjectByName(PUNICODE_STRING ObjectPath,
                                                  ULONG Attributes,
                                                  PACCESS_STATE PassedAccessState,
                                                  ACCESS_MASK DesiredAccess,
                                                  POBJECT_TYPE ObjectType,
                                                  KPROCESSOR_MODE AccessMode,
                                                  PVOID ParseContext,
                                                  PVOID* ObjectPtr);

#include <ntnative/zw_utility.hpp>
#include <ntnative/zw_file.hpp>
#include <ntkrnl/klock.hpp>
#include <ntkrnl/ktree.hpp>

void PrintEx(const char* format, ...);

class Driver
{
public:
    DELETE_COPY(Driver);
    DELETE_MOVE(Driver);

	virtual ~Driver() = default;
	
	PDRIVER_OBJECT GetRawDriverObject() const;

    virtual NTSTATUS Initialize();
    virtual PFLT_FILTER GetRawFilterObject() const;
    virtual bool IsMiniFilter() const;
    
protected:
	Driver(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path, bool is_unloadable = true);
	
    virtual VOID _OnUnloadDriver();
    static  VOID _UnloadDriver(PDRIVER_OBJECT drv_obj);

protected:
	PDRIVER_OBJECT _drv_obj;
};

extern Driver* g_drv;

inline bool IsMiniFilter()
{
    return g_drv->IsMiniFilter();
}

inline ULONG GetRandomSeed()
{
    LARGE_INTEGER n = {};
    KeQueryPerformanceCounter(&n);

    return n.LowPart;
}

Driver* CreateDriverInstance(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path); // The user must implement this function

template<class T, ENABLE_IF(IsFundamental<T>::value || IsPointer<T>::value)>
class SimpleTreeNode final : public ITreeNode
{
public:
    bool operator <(const ITreeNode& other) const override
    {
        return _value < static_cast<const SimpleTreeNode&>(other)._value;
    }

    bool operator ==(const ITreeNode& other) const override
    {
        return _value == static_cast<const SimpleTreeNode&>(other)._value;
    }

    int GetNodeType(const ITreeNode& other) const override
    {
        return 0; // unused in this class
    }

public:
    SimpleTreeNode()
        : _value()
    {}

    SimpleTreeNode(T value)
        : _value(value)
    {}

    SimpleTreeNode& operator =(T value)
    {
        _value = value;

        return *this;
    }

    bool operator >(const ITreeNode& other) const
    {
        return _value > static_cast<const SimpleTreeNode&>(other)._value;
    }

    bool operator !=(const ITreeNode& other) const
    {
        return _value != static_cast<const SimpleTreeNode&>(other)._value;
    }

    bool operator <=(const ITreeNode& other) const
    {
        return _value <= static_cast<const SimpleTreeNode&>(other)._value;
    }

    bool operator >=(const ITreeNode& other) const
    {
        return _value >= static_cast<const SimpleTreeNode&>(other)._value;
    }

    operator T() const
    {
        return _value;
    }
    
private:
    T _value;
};

class DriverNotifyingProcessThreadCreateAndExit : public Driver
{
protected:
    DriverNotifyingProcessThreadCreateAndExit(PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path, bool is_unloadable = true)
        : Driver(drv_obj, reg_path, is_unloadable), _new_pids()
    {
        PsSetLoadImageNotifyRoutine(_OnLoadImage);
        PsSetCreateProcessNotifyRoutine(_OnProcessCreateOrExit, FALSE);
        PsSetCreateThreadNotifyRoutine(_OnThreadCreateOrExit);
    }

    ~DriverNotifyingProcessThreadCreateAndExit()
    {
        PsSetCreateProcessNotifyRoutine(_OnProcessCreateOrExit, TRUE);
        PsRemoveLoadImageNotifyRoutine(_OnLoadImage);
        PsRemoveCreateThreadNotifyRoutine(_OnThreadCreateOrExit);
    }

protected:
    virtual void _NotifyProcessCreate(ProcessId parent_pid, ProcessId pid)
    {}

    virtual void _NotifyProcessImagePath(ProcessId pid, PUNICODE_STRING process_path)
    {}

    virtual void _NotifyProcessExit(ProcessId pid)
    {}

    virtual void _NotifyThreadCreate(HANDLE pid, HANDLE tid)
    {}

    virtual void _NotifyThreadExit(HANDLE pid, HANDLE tid)
    {}

private:
    static VOID NTAPI _OnLoadImage(PUNICODE_STRING img_path, HANDLE pid, PIMAGE_INFO img_info)
    {
        auto self_drv = static_cast<DriverNotifyingProcessThreadCreateAndExit*>(g_drv);

        bool is_found = false;

        if (self_drv->_new_pids.Lookup(pid))
        {
            self_drv->_NotifyProcessImagePath(pid, img_path);
            is_found = true;
        }

        if (is_found)
        {
            self_drv->_new_pids.Delete(pid);
        }
    }

    static VOID NTAPI _OnProcessCreateOrExit(HANDLE parent_pid, HANDLE pid, BOOLEAN is_create)
    {
        auto self_drv = static_cast<DriverNotifyingProcessThreadCreateAndExit*>(g_drv);

        if (is_create)
        {
            self_drv->_new_pids.Insert(pid);
            self_drv->_NotifyProcessCreate(parent_pid, pid);
        }
        else
        {            
            self_drv->_NotifyProcessExit(pid);
        }
    }

    static VOID NTAPI _OnThreadCreateOrExit(HANDLE pid, HANDLE tid, BOOLEAN b_create)
    {
        auto self_drv = static_cast<DriverNotifyingProcessThreadCreateAndExit*>(g_drv);

        if (b_create)
        {
            self_drv->_NotifyThreadCreate(pid, tid);
            self_drv->_new_pids.Insert(tid);
        }
        else
        {
            self_drv->_NotifyThreadExit(pid, tid);
            self_drv->_new_pids.Delete(tid);
        }
    }

private:
    SplayTree<SimpleTreeNode<ProcessId>, EResource> _new_pids;
};