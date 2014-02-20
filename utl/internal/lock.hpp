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

class GuardedLock_
{
protected:
    GuardedLock_(ILock* lock)
        : _lock(lock)
    {}

    ~GuardedLock_()
    {
        _lock->Release();
    }

protected:
    ILock* _lock;
};

struct GuardedLockExclusive_ : GuardedLock_
{
    GuardedLockExclusive_(ILock* lock)
        : GuardedLock_(lock)
    {
        _lock->AcquireExclusive();
    }
};

struct GuardedLockShared_ : GuardedLock_
{
    GuardedLockShared_(ILock* lock)
        : GuardedLock_(lock)
    {
        _lock->AcquireShared();
    }
};

#define LOCK_EXCLUSIVELY(lock) GuardedLockExclusive_ UID_ (&lock)
#define LOCK_SHAREDLY(lock)    GuardedLockShared_    UID_ (&lock)
