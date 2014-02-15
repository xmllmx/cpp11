#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4425) // 'const size_t Array<T,t_size>::size(void) const' : 'constexpr' was ignored (class literal types are not yet supported)
#endif

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

typedef unsigned char Byte;

#include <internal/macro.hpp>
#include <internal/traits.hpp>
#include <internal/relop.hpp>
#include <internal/functional.hpp>
#include <internal/memory.hpp>
#include <internal/array.hpp>
#include <internal/tuple.hpp>
#include <internal/utility.hpp>
#include <internal/string.hpp>