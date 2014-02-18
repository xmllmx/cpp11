#pragma once

INTERFACE ILock
{
    virtual ~ILock() = default;

    virtual void AcquireExclusive() = 0;
    virtual void AcquireShared() = 0;
    virtual void Release() = 0;
};

INTERFACE IReentrantLock : ILock
{
    static const bool is_reentrant = true;
};

INTERFACE INonReentrantLock : ILock
{
    static const bool is_reentrant = false;
};

struct NullLock final : IReentrantLock
{
    void AcquireExclusive() override {}
    void AcquireShared() override {}
    void Release() override {}
};

class _GuardedLock_
{
protected:
    _GuardedLock_(ILock* lock)
        : _lock(lock)
    {}

    ~_GuardedLock_()
    {
        _lock->Release();
    }

protected:
    ILock* _lock;
};

struct _GuardedLockExclusive_ : _GuardedLock_
{
    _GuardedLockExclusive_(ILock* lock)
        : _GuardedLock_(lock)
    {
        _lock->AcquireExclusive();
    }
};

struct _GuardedLockShared_ : _GuardedLock_
{
    _GuardedLockShared_(ILock* lock)
        : _GuardedLock_(lock)
    {
        _lock->AcquireShared();
    }
};

#define LOCK_EXCLUSIVELY(lock) _GuardedLockExclusive_ _UID_ (&lock)
#define LOCK_SHAREDLY(lock)    _GuardedLockShared_    _UID_ (&lock)
