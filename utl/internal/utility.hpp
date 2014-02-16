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