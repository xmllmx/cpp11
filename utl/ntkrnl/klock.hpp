#pragma once

class SpinLock : public INonReentrantLock
{
public:
    SpinLock()
        : _spin_lock(), _old_irql()
    {
        KeInitializeSpinLock(&_spin_lock);
    }

    void AcquireExclusive() override
    {
        KeAcquireSpinLock(&_spin_lock, &_old_irql);
    }

    void AcquireShared() override
    {
        AcquireExclusive();
    }

    void Release() override;

private:
    KSPIN_LOCK _spin_lock;
    KIRQL      _old_irql;
};

class FastMutex : public INonReentrantLock
{
public:
    FastMutex()
        : _fast_mtx()
    {
        ExInitializeFastMutex(&_fast_mtx);
    }

    void AcquireExclusive() override
    {
        PAGED_CODE();
        ExAcquireFastMutex(&_fast_mtx);
    }

    void AcquireShared() override
    {
        AcquireExclusive();
    }

    void Release() override
    {
        PAGED_CODE();
        ExReleaseFastMutex(&_fast_mtx);
    }

private:
    FAST_MUTEX _fast_mtx;
};

class EResource final : public IReentrantLock
{
public:
    EResource()
        : _res()
    {
        ExInitializeResourceLite(&_res);
    }

    ~EResource() override
    {
        PAGED_CODE();
        ExDeleteResourceLite(&_res);
    }

    void AcquireExclusive() override
    {
        PAGED_CODE();

        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite(&_res, TRUE);
    }

    void AcquireShared() override
    {
        PAGED_CODE();

        KeEnterCriticalRegion();
        ExAcquireResourceSharedLite(&_res, TRUE);
    }

    void Release() override
    {
        PAGED_CODE();

        ExReleaseResourceLite(&_res);
        KeLeaveCriticalRegion();
    }

private:
    ERESOURCE _res;
};