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

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
constexpr bool IsBitOn(T bits, uint64_t offset)
{
    return 1 == ((bits << offset) >> (sizeof(bits) * CHAR_BIT - 1));
}

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
constexpr bool IsBitOff(T bits, uint64_t offset)
{
    return !IsBitOn(bits, offset);
}

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
void SetBitOn(T& bits, uint64_t offset)
{
    T n = 1;
    n <<= sizeof(n)* CHAR_BIT - 1;
    n >>= offset;

    bits |= n;
}

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
void SetBitOff(T& bits, uint64_t offset)
{
    T n = 1;
    n <<= sizeof(n)* CHAR_BIT - 1;
    n >>= offset;
    n = ~n;

    bits &= n;
}

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
uint8_t GetCountOfBitOn(T bits)
{
    uint8_t count = 0;

    FOR(i, sizeof(bits) * CHAR_BIT)
    {
        count += IsBitOn(bits, i);
    }

    return count;
}

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
uint8_t GetCountOfBitOff(T bits)
{
    uint8_t count = 0;

    FOR(i, sizeof(value) * CHAR_BIT)
    {
        count += IsBitOff(bits);
    }

    return count;
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

template<class T, size_t t_capacity>
void Zero(T(&array_obj)[t_capacity])
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
    ENABLE_IF(IsSame<StringType<CharType>, String<CharType>>::value || IsSame<StringType<CharType>, StringRef<CharType>>::value)>
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
bool BeginsWith(const StringType<CharType>& str, const CharType* sub_str, size_t sub_str_length, bool is_case_sensitive = true)
{
    if (nullptr == sub_str || 0 == sub_str_length)
    {
        return false;
    }

    if (str.length() < sub_str_length)
    {
        return false;
    }

    return AreEqual(StringRef<CharType>(str.data(), sub_str_length), StringRef<CharType>(sub_str, sub_str_length), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool BeginsWith(const StringType<CharType>& str, const StringType<CharType>& sub_str, bool is_case_sensitive = true)
{
    return BeginsWith(str, sub_str.data(), sub_str.length(), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool BeginsWith(const StringType<CharType>& str, const CharType* sub_str, bool is_case_sensitive = true)
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
bool BeginsWith(const CharType* str, const CharType* sub_str, bool is_case_sensitive = true)
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
bool EndsWith(const StringType<CharType>& str, const CharType* sub_str, size_t sub_str_length, bool is_case_sensitive = true)
{
    if (nullptr == sub_str || 0 == sub_str_length)
    {
        return false;
    }

    if (str.length() < sub_str_length)
    {
        return false;
    }

    return AreEqual(StringRef<CharType>(&str[str.length() - sub_str_length], sub_str_length), StringRef<CharType>(sub_str, sub_str_length), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool EndsWith(const StringType<CharType>& str, const StringType<CharType>& sub_str, bool is_case_sensitive = true)
{
    return EndsWith(str, &sub_str[0], sub_str.length(), is_case_sensitive);
}

template<template<class> class StringType, class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool EndsWith(const StringType<CharType>& str, const CharType* sub_str, bool is_case_sensitive = true)
{
    return EndsWith(str, sub_str.get(), GetLength(sub_str), is_case_sensitive);
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
bool EndsWith(const CharType* str, const CharType* sub_str, bool is_case_sensitive = true)
{
    return EndsWith(StringRef<CharType>(str, GetLength(str)), StringRef<CharType>(sub_str, GetLength(sub_str)), is_case_sensitive);
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

template<class CharType, ENABLE_IF(IsCharType<CHARTYPE>::value)>
bool IsPathSeparator(CharType c)
{
    return CharType('\\') == c || CharType('/') == c;
}

template<template<class> class StringType, class CharType, 
    ENABLE_IF(IsSame<StringType<CharType>, String<CharType>>::value || IsSame<StringType<CharType>, StringRef<CharType>>::value)>
bool SubstringExists(const StringType<CharType>& str, const CharType* sub_str)
{
    return SubstringExists(str.data(), sub_str);
}

class BoolStatusChecker
{
public:
    typedef bool StatusType;
    static const bool default_status = false;

    bool operator ()(bool b) const
    {
        return b;
    }
};

template<class T, class StatusCheckerType = BoolStatusChecker, ENABLE_IF(IsIntegral<StatusCheckerType::StatusType>::value)>
class Optional final
{
public:
    typedef typename StatusCheckerType::StatusType StatusType;

    DEFAULT_COPY(Optional);
    DEFAULT_MOVE(Optional);
    Optional() = default;
    
    explicit operator bool() const
    {
        return _fn_status_checker(_status);
    }

public:
    template<ENABLE_IF(!IsGreaterThan(sizeof(T), sizeof(void*)))>
    Optional(T value, StatusType status = StatusCheckerType::default_status)
        : _value(value), _status(status), _fn_status_checker()
    {}
    
    template<ENABLE_IF(IsGreaterThan(sizeof(T), sizeof(void*)))>
    Optional(const T& value, StatusType status = StatusCheckerType::default_status)
        : _value(value), _status(status), _fn_status_checker()
    {}

    Optional(T&& value, StatusType status = StatusCheckerType::default_status)
        : _value(Move(value)), _status(status), _fn_status_checker()
    {}

    const T* get() const
    {
        Assert(*this);

        return &_value;
    }

    T* get()
    {
        Assert(*this);

        return &_value;
    }

    StatusType status() const
    {
        return _status;
    }

    const T* operator ->() const
    {
        Assert(*this);

        return &_value;
    }

    T* operator ->()
    {
        Assert(*this);

        return &_value;
    }
    
    const T& operator *() const
    {
        Assert(*this);

        return _value;
    }

    T& operator *()
    {
        Assert(*this);

        return _value;
    }

private:
    T                 _value;
    StatusType        _status;
    StatusCheckerType _fn_status_checker;
};

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
inline CharType tolower(CharType c)
{
    if (c <= CharType('Z') && c >= CharType('A'))
    {
        c = CharType(CharType('a') + (c - CharType('A')));
    }

    return c;
}

template<class CharType, ENABLE_IF(IsCharType<CharType>::value)>
inline CharType toupper(CharType c)
{
    if (c <= CharType('z') && c >= CharType('a'))
    {
        c = CharType(CharType('A') + (c - CharType('a')));
    }

    return c;
}
