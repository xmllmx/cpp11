#pragma once

#ifndef INTERFACE
/**/#define INTERFACE struct
#endif

#ifndef IN
/**/#define IN
#endif

#ifndef OUT
/**/#define OUT
#endif

#ifndef OPTIONAL
/**/#define OPTIONAL
#endif

#define _LINE_NUM_AS_ID_RESULT_(macro_name) _line_num_as_id_##macro_name##_
#define _LINE_NUM_AS_ID_(macro_name)        _LINE_NUM_AS_ID_RESULT_(macro_name)
#define _UID_                               _LINE_NUM_AS_ID_(__LINE__)

#define _UIND_IN_LINE_RESULT_(uid, n) uid##n
#define _UID_IN_LINE_(uid, n)         _UIND_IN_LINE_RESULT_(uid, n)
#define _UID_N_(n)                    _UID_IN_LINE_(_UID_, n)

//
// WIDE_LITERAL is useful when you want to use an existing narrow-literal to act as a wide-literal.
//
#define _WIDE_LITERAL_(string_literal) L##string_literal
#define WIDE_LITERAL(string_literal)   _WIDE_LITERAL_(string_literal)

#define _LITERAL_SIZE_NOT_INCLUDING_NULL_TERMINATOR_(str_literal) ((sizeof(str_literal str_literal) - sizeof(str_literal[0])) / 2)

#define GET_LITERAL_LENGTH(str_literal) (_LITERAL_SIZE_NOT_INCLUDING_NULL_TERMINATOR_(str_literal) / sizeof(str_literal[0]))
#define GET_LITERAL_SIZE(str_literal)   (GET_LITERAL_LENGTH(str_literal) * sizeof(str_literal[0]))

#define IS_POWER_OF_2(x) (((x) != 0) && (((x) & ((x) - 1)) == 0))
#define OFFSET_OF(Type, field) ((size_t)&(((##Type##*)0)->##field##))
#define IS_INTEGER_MULTIPLE(dividend, divisor) (0 == (dividend) % (divisor))

#define C_DECL_BEGIN() extern "C" {
#define C_DECL_END() }

#define ANS_BEGIN() namespace {
#define NS_BEGIN(ns_name) namespace ns_name {
#define ANS_END() }
#define NS_END() }

#define LOOP(n) for (decltype(n) _dummy_ = 0; _dummy_ < (n); ++_dummy_)
#define FOR(i, n) for (RemoveConstVolatileReference<decltype((n))>::type i = 0; i < (n); ++i)

#define XOR(a, b) (((a) && !(b)) || (!(a) && (b)))
#define XNOR(a, b) (((a) && (b)) || (!(a) && !(b)))

#define SAFE_FREE(ptr) { Free(ptr); ptr = 0; }
#define SAFE_DELETE(ptr) { delete ptr; ptr = 0; }
#define SAFE_DELETE_ARRAY(ptr) { delete[] ptr; ptr = 0; }

#define DELETE_COPY(class_name) class_name(const class_name &) = delete; class_name& operator =(const class_name &) = delete
#define DELETE_MOVE(class_name) class_name(class_name &&) = delete; class_name& operator =(class_name &&) = delete

#define DEFAULT_COPY(class_name) class_name(const class_name &) = default; class_name& operator =(const class_name &) = default
#define DEFAULT_MOVE(class_name) class_name(class_name &&) = default; class_name& operator =(class_name &&) = default

#define DEFINE_COPY_ASSIGNER(class_name) class_name & operator =(const class_name & other) { \
    if (this != &other) { \
    class_name tmp(other); \
    this->swap(tmp); } \
    return *this; }

#define DEFINE_MOVE_ASSIGNER(class_name) class_name & operator =(class_name && other) { \
    if (this != &other) { \
    class_name tmp(Move(other)); \
    this->swap(tmp); } \
    return *this; }

#define AssertIf(condition, expr) Assert((condition) ? !!(expr) : true)
