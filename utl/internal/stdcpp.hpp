#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <climits> // for CHAR_BITS

#if defined(_MSC_VER) && !defined(_NOEXCEPT)
#define _NOEXCEPT noexcept
#include <initializer_list>
#undef _NOEXCEPT
#else
#include <initializer_list>
#endif

#include <internal/macro.hpp>
#include <internal/traits.hpp>
#include <internal/functional.hpp>
#include <internal/memory.hpp>

typedef unsigned char Byte;

template<class T>
struct DeduciedType final
{
    typedef T type;
};

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
const T& Min(const T& a, const T& b)
{
    return a < b ? a : b;
}

template<class T, ENABLE_IF(sizeof(T) > sizeof(void*))>
const T& Max(const T& a, const T& b)
{
    return a < b ? b : a;
}

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
T Min(T a, T b)
{
    return a < b ? a : b;
}

template<class T, ENABLE_IF(sizeof(T) <= sizeof(void*))>
T Max(T a, T b)
{
    return a < b ? b : a;
}

template<class PointerType = void*>
PointerType MovePtr(const void* p, int64_t offset_in_bytes)
{
    auto tmp_p = const_cast<Byte*>(static_cast<const Byte*>(p)+offset_in_bytes);

    return reinterpret_cast<PointerType>(tmp_p);
}

template<class T>
size_t PtrDiffInElements(const T* p_1, const T* p_2)
{
    return p_1 < p_2 ? p_2 - p_1 : p_1 - p_2;
}

inline size_t PtrDiffInBytes(const void* p_1, const void* p_2)
{
    auto p_a = static_cast<const Byte*>(p_1);
    auto p_b = static_cast<const Byte*>(p_2);

    return p_a < p_b ? p_b - p_a : p_a - p_b;
}

template<class T, size_t t_capacity>
size_t GetArraySize(const T(&)[t_capacity])
{
    return t_capacity;
}

inline bool IsAnsiCharU16(wchar_t c)
{
    return c < 0x128;
}

inline bool IsChineseCharU16(wchar_t c)
{
    return (c >= 0x4E00 && c <= 0x9FA5) || (c >= 0xF900 && c <= 0xFA2D);
}

