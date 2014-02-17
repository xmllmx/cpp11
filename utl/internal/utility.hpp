#pragma once

template<class T>
struct Deduced final
{
    typedef T type;
};

template<class PointerType = void*>
constexpr PointerType MovePtr(void* p, int64_t offset_in_bytes)
{
    return reinterpret_cast<PointerType>(static_cast<Byte*>(p)+offset_in_bytes);
}

template<class PointerType = const void*>
constexpr PointerType MovePtr(const void* p, int64_t offset_in_bytes)
{
    return reinterpret_cast<PointerType>(static_cast<const Byte*>(p)+offset_in_bytes);
}

template<class T>
constexpr size_t PtrDiffInElements(const T* p_1, const T* p_2)
{
    return p_1 < p_2 ? p_2 - p_1 : p_1 - p_2;
}

inline size_t PtrDiffInBytes(const void* p_1, const void* p_2)
{
    auto p_a = static_cast<const Byte*>(p_1);
    auto p_b = static_cast<const Byte*>(p_2);

    return p_a < p_b ? p_b - p_a : p_a - p_b;
}

inline constexpr bool IsAnsiChar(wchar_t c)
{
    return c < 0x128;
}

inline constexpr bool IsChineseCharU16(wchar_t c)
{
    return (c >= 0x4E00 && c <= 0x9FA5) || (c >= 0xF900 && c <= 0xFA2D);
}

template<class UnsignedInteger>
constexpr bool IsBitOn(UnsignedInteger bits, uint64_t offset)
{
    static_assert(IsUnsigned<UnsignedInteger>::value && IsIntegral<UnsignedInteger>::value, "UnsignedInteger must be an unsigned integral type");

    bits <<= offset;
    bits >>= sizeof(bits)* CHAR_BIT - 1;

    return 1 == bits;
}

template<class UnsignedInteger>
constexpr bool IsBitOff(UnsignedInteger bits, uint64_t offset)
{
    return !IsBitOn(bits, offset);
}

template<class UnsignedInteger>
void SetBitOn(UnsignedInteger& bits, uint64_t offset)
{
    static_assert(IsUnsigned<UnsignedInteger>::value && IsIntegral<UnsignedInteger>::value, "UnsignedInteger must be an unsigned integral type");

    UnsignedInteger n = 1;
    n <<= sizeof(n)* CHAR_BIT - 1;
    n >>= offset;

    bits |= n;
}

template<class UnsignedInteger>
void SetBitOff(UnsignedInteger& bits, uint64_t offset)
{
    static_assert(IsUnsigned<UnsignedInteger>::value && IsIntegral<UnsignedInteger>::value, "UnsignedInteger must be an unsigned integral type");

    UnsignedInteger n = 1;
    n <<= sizeof(n)* CHAR_BIT - 1;
    n >>= offset;
    n = ~n;

    bits &= n;
}

template<class FlagEnum>
constexpr FlagEnum operator |(FlagEnum a, FlagEnum b)
{
    static_assert(IsEnum<FlagEnum>::value, "FlagEnum is not an enum type");

    return static_cast<FlagEnum>(int(a) | int(b));
}

template<class FlagEnum>
constexpr FlagEnum operator |=(FlagEnum a, FlagEnum b)
{
    static_assert(IsEnum<FlagEnum>::value, "FlagEnum is not an enum type");

    return static_cast<FlagEnum>(int(a) |= int(b));
}

template<class T>
constexpr bool IsAlignedTo(T n, size_t alignment)
{
    static_assert(IsPointer<T>::value || IsIntegral<T>::value, "T must be a pointer type or a integral type");

    return 0 == ((uint64_t)(n)& (alignment - 1));
}

template<size_t t_alignment, class T>
constexpr bool IsAlignedTo(T n)
{
    static_assert(IsPointer<T>::value || IsIntegral<T>::value, "T must be a pointer type or a integral type");
    static_assert(IS_POWER_OF_2(t_alignment), "invalid alignment size");

    return 0 == ((uint64_t)(n)& (t_alignment - 1));
}

template<uint64_t t_alignment, class Integer>
constexpr Integer RoundUpTo(Integer n)
{
    static_assert(IsIntegral<Integer>::value, "Integer must be a integral type");
    static_assert(IS_POWER_OF_2(t_alignment), "invalid alignment size");

    return (uint64_t(n) + (t_alignment - 1)) & ~(t_alignment - 1);
}

template<uint64_t t_alignment, class Integer>
constexpr Integer RoundDownTo(Integer n)
{
    static_assert(IsIntegral<Integer>::value, "Integer must be a integral type");
    static_assert(IS_POWER_OF_2(t_alignment), "invalid alignment size");

    return uint64_t(n) & ~(t_alignment - 1);
}

template<class Integer>
constexpr Integer RoundUp(Integer n, uint64_t alignment)
{
    static_assert(IsIntegral<Integer>::value, "Integer must be a integral type");
    Assert(IS_POWER_OF_2(alignment));

    return (uint64_t(n) + (alignment - 1)) & ~(alignment - 1);
}

template<class Integer>
constexpr Integer RoundDown(Integer n, uint64_t alignment)
{
    static_assert(IsIntegral<Integer>::value, "Integer must be a integral type");
    Assert(IS_POWER_OF_2(alignment));

    return uint64_t(n) & ~(alignment - 1);
}

template<class T1, class T2>
constexpr bool IsFlagOn(T1 flags, T2 flag)
{
    return !!(uint64_t(flags) & uint64_t(flag));
}

template<class T1, class T2>
constexpr bool IsFlagOff(T1 flags, T2 flag)
{
    return !IsFlagOn(flags, flag);
}

template<class T1, class T2>
T1 SetFlagOn(T1& flags, T2 flag)
{
    *static_cast<uint64_t*>(static_cast<void*>(&flags)) |= uint64_t(flag);

    return flags;
}

template<class T1, class T2>
T1 SetFlagOff(T1& flags, T2 flag)
{
    *static_cast<uint64_t*>(static_cast<void*>(&flags)) &= ~uint64_t(flag);

    return flags;
}

inline void Zero(void* p, size_t size)
{
    memset(p, 0, size);
}

template<class T, size_t t_size>
void Zero(T(&array_obj)[t_size])
{
    Zero(array_obj, sizeof(array_obj));
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
inline bool AreEqual(CharType a, CharType b, bool is_case_sensitive = true)
{
    if (is_case_sensitive)
    {
        return a == b;
    }
    
    if (IsAnsiChar(a) && IsAnsiChar(b))
    {
        return tolower(a) == tolower(b);
    }

    return false;
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
inline bool AreEqual(const CharType* a, const CharType* b, bool is_case_sensitive = true)
{
    if (a == b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    if (is_case_sensitive)
    {
        FOR(i, Max<size_t>())
        {
            if (a[i] != b[i])
            {
                return false;
            }
            
            if (0 == a[i])
            {
                return true;
            }
        }
    }
    else
    {
        FOR(i, Max<size_t>())
        {
            if (IsAnsiChar(a[i]) && IsAnsiChar(b[i]))
            {
                if (tolower(a[i]) != tolower(b[i]))
                {
                    return false;
                }
            }
            else
            {
                if (a[i] != b[i])
                {
                    return false;
                }
            }

            if (0 == a[i])
            {
                return true;
            }
        }
    }

    return false;
}

template<template<class> class StringType, class CharType, 
    ENABLE_IF(IsSame<StringType<CharType>, BasicString<CharType>>::value || IsSame<StringType<CharType>, RawString<CharType>>::value)>
bool AreEqual(const StringType<CharType>& str_a, const StringType<CharType>& str_b, bool is_case_sensitive = true)
{
    if (str_a.length() == str_b.length())
    {
        FOR(i, str_a.length())
        {
            if (!AreEqual(str_a[0], str_b[0], is_case_sensitive))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsBeginningWith(const StringType<CharType>& str, const CharType* sub_str, size_t sub_str_length, bool is_case_sensitive = true)
{
    if (nullptr == sub_str || 0 == sub_str_length)
    {
        return false;
    }

    if (str.length() < sub_str_length)
    {
        return false;
    }

    return AreEqual(RawString<CharType>(str.data(), sub_str_length), RawString<CharType>(sub_str, sub_str_length), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsBeginningWith(const StringType<CharType>& str, const StringType<CharType>& sub_str, bool is_case_sensitive = true)
{
    return IsBeginningWith(str, sub_str.data(), sub_str.length(), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsBeginningWith(const StringType<CharType>& str, const CharType* sub_str, bool is_case_sensitive = true)
{
    auto length = str.length();

    FOR(i, length)
    {
        if (0 == sub_str[i])
        {
            return true;
        }

        if (is_case_sensitive)
        {
            if (str[i] != sub_str[i])
            {
                return false;
            }
        }
        else
        {
            if (!AreEqual(str[i], sub_str[i], false))
            {
                return false;
            }
        }
    }

    return false;
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsBeginningWith(const CharType* str, const CharType* sub_str, bool is_case_sensitive = true)
{
    auto length = Max<size_t>();

    FOR(i, length)
    {
        if (0 == sub_str[i])
        {
            return true;
        }

        if (0 == str[i])
        {
            return false;
        }

        if (is_case_sensitive)
        {
            if (str[i] != sub_str[i])
            {
                return false;
            }
        }
        else
        {
            if (!AreEqual(str[i], sub_str[i], false))
            {
                return false;
            }
        }
    }

    return false;
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsEndingWith(const StringType<CharType>& str, const CharType* sub_str, size_t sub_str_length, bool is_case_sensitive = true)
{
    if (nullptr == sub_str || 0 == sub_str_length)
    {
        return false;
    }

    if (str.length() < sub_str_length)
    {
        return false;
    }

    return AreEqual(RawString<CharType>(&str[str.length() - sub_str_length], sub_str_length), RawString<CharType>(sub_str, sub_str_length), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsEndingWith(const StringType<CharType>& str, const StringType<CharType>& sub_str, bool is_case_sensitive = true)
{
    return IsEndingWith(str, &sub_str[0], sub_str.length(), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsEndingWith(const StringType<CharType>& str, const CharType* sub_str, bool is_case_sensitive = true)
{
    return IsEndingWith(str, sub_str.get(), GetLength(sub_str), is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool IsEndingWith(const CharType* str, const CharType* sub_str, bool is_case_sensitive = true)
{
    return IsEndingWith(RawString<CharType>(str, GetLength(str)), RawString<CharType>(sub_str, GetLength(sub_str)), is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool SubstringExists(const CharType* str, const CharType* sub_str)
{
    if (1 == sizeof(CharType))
    {
        return !!strstr(reinterpret_cast<char*>(str), reinterpret_cast<char*>(sub_str));
    }
    else
    {
        return !!wcsstr(reinterpret_cast<char*>(str), reinterpret_cast<char*>(sub_str));
    }
}

template<template<class> class StringType, class CharType, 
    ENABLE_IF(IsSame<StringType<CharType>, BasicString<CharType>>::value || IsSame<StringType<CharType>, RawString<CharType>>::value)>
bool SubstringExists(const StringType<CharType>& str, const CharType* sub_str)
{
    return SubstringExists(str.data(), sub_str);
}

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
