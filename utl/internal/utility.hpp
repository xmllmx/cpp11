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
