#pragma once

struct TrueType
{
	enum { value = true };
};

struct FalseType
{
	enum { value = false };
};

template<class T>
struct IsConst : FalseType
{};

template<class T>
struct IsConst<const T> : TrueType
{};

template<class T>
struct IsCharType : FalseType {};

template<>
struct IsCharType<char> : TrueType {};

template<>
struct IsCharType<const char> : TrueType {};

template<>
struct IsCharType<wchar_t> : TrueType {};

template<>
struct IsCharType<const wchar_t> : TrueType {};

template<class T>
struct IsCompatibleNarrowChar
{
    enum { value = sizeof(char) == sizeof(T) };
};

template<class T>
struct IsCompatibleWideChar
{
    enum { value = sizeof(wchar_t) == sizeof(T) };
};

template<class T>
struct IsCompatibleCharType
{
    enum { value = IsCompatibleNarrowChar<T>::value || IsCompatibleWideChar<T>::value };
};

template<bool test>
struct EnableIf
{
	// type is undefined for assumed !test
};

template<>
struct EnableIf<true>
{
	typedef void type; // type is DummyType for test
};

#define _ENABLE_IF_(expr) class = typename EnableIf<expr>::type
#define ENABLE_IF(...) _ENABLE_IF_((__VA_ARGS__))

template<class CharType>
struct IsOrdinaryChar
{
    enum 
    {
        value = 1 == sizeof(CharType)
    };
};

template<class T>
struct IsArray : FalseType
{};

template<class T, size_t t_array_size>
struct IsArray<T[t_array_size]> : TrueType
{};

template<class T>
struct IsArray<T[]> : TrueType
{};

template<class T>
struct IsFundamental : FalseType
{};

#define _IS_FUNDAMENTAL_(T) template<> struct IsFundamental<T> : TrueType {}

_IS_FUNDAMENTAL_(bool);
_IS_FUNDAMENTAL_(char);
_IS_FUNDAMENTAL_(wchar_t);

_IS_FUNDAMENTAL_(signed char);
_IS_FUNDAMENTAL_(unsigned char);

_IS_FUNDAMENTAL_(signed short int);
_IS_FUNDAMENTAL_(unsigned short int);

_IS_FUNDAMENTAL_(signed int);
_IS_FUNDAMENTAL_(unsigned int);

_IS_FUNDAMENTAL_(signed long int);
_IS_FUNDAMENTAL_(unsigned long int);

_IS_FUNDAMENTAL_(signed long long int);
_IS_FUNDAMENTAL_(unsigned long long int);

_IS_FUNDAMENTAL_(float);
_IS_FUNDAMENTAL_(double);
_IS_FUNDAMENTAL_(long double);

template<class T>
struct IsPointer : FalseType
{};

template<class T>
struct IsPointer<T*> : TrueType
{};

template<class T>
struct IsEnum
{
    enum { value = __is_enum(T) };
};

template<class T>
struct IsPod
{
	enum { value = __is_pod(T) };
};

template<class T>
struct IsStandardLayout
{
    enum { value = __is_standard_layout(T) };
};

template<class T>
struct IsTrivial
{
    enum { value = __is_trivial(T) };
};

template<class T>
struct IsCType
{
	enum { value = IsPod<T>::value||IsFundamental<T>::value||IsPointer<T>::value };
};

template<class T>
struct RemovePointer
{
	typedef T type;
};

template<class T>
struct RemovePointer<T*>
{
	typedef T type;
};

template<class T>
struct RemoveReference
{
	typedef T type;
};

template<class T>
struct RemoveReference<T&>
{
	typedef T type;
};

template<class T>
struct RemoveReference<T&&>
{
	typedef T type;
};

template<class T>
inline typename RemoveReference<T>::type&& Move(T&& obj)
{
	return static_cast<typename RemoveReference<T>::type&&>(obj);
}

template<class T> 
inline T&& Forward(typename RemoveReference<T>::type& arg)
{
    return static_cast<T&&>(arg);
}

template<class T> 
inline T&& Forward(typename RemoveReference<T>::type&& arg)
{
	return static_cast<T&&>(arg);
}

template<class T>
struct AddLvalueReference
{
	typedef typename RemoveReference<T>::type& type;
};

template<class T>
struct AddRvalueReference
{
    typedef typename RemoveReference<T>::type&& type;
};

template<class T>
struct RemoveConst
{
	typedef T type;
};

template<class T>
struct RemoveConst<const T>
{
	typedef T type;
};

template<class T>
struct RemoveConst<const T[]>
{
	typedef T type[];
};

template<class T, size_t t_array_size>
struct RemoveConst<const T[t_array_size]>
{
	typedef T type[t_array_size];
};

template<class T>
struct RemoveVolatile
{
    typedef T type;
};

template<class T>
struct RemoveVolatile<volatile T>
{
    typedef T type;
};

template<class T>
struct RemoveVolatile<volatile T[]>
{
    typedef T type[];
};

template<class T, size_t t_array_size>
struct RemoveVolatile<volatile T[t_array_size]>
{
    typedef T type[t_array_size];
};

template<class T>
struct RemoveConstVolatile
{
    typedef typename RemoveConst<typename RemoveVolatile<T>::type>::type type;
};

template<class T>
struct RemoveConstVolatileReference
{
    typedef typename RemoveReference<typename RemoveConstVolatile<T>::type>::type type;
};

template<class T>
struct RemoveConstVolatileReferencePointer
{
    typedef typename RemovePointer<typename RemoveConstVolatileReference<T>::type>::type type;
};

template<class T> using Raw = RemoveConstVolatileReferencePointer<T>;

template<class T>
struct IsFloatingPoint : FalseType
{};

template<>
struct IsFloatingPoint<float> : TrueType
{};

template<>
struct IsFloatingPoint<double> : TrueType
{};

template<>
struct IsFloatingPoint<long double> : TrueType
{};

template<class T> struct IsIntegral : FalseType {};

template<> struct IsIntegral<bool>               : TrueType {};
template<> struct IsIntegral<char>               : TrueType {};
template<> struct IsIntegral<short>              : TrueType {};
template<> struct IsIntegral<int>                : TrueType {};
template<> struct IsIntegral<long>               : TrueType {};
template<> struct IsIntegral<long long>          : TrueType {};
template<> struct IsIntegral<unsigned char>      : TrueType {};
template<> struct IsIntegral<unsigned short>     : TrueType {};
template<> struct IsIntegral<unsigned int>       : TrueType {};
template<> struct IsIntegral<unsigned long>      : TrueType {};
template<> struct IsIntegral<unsigned long long> : TrueType {};

template<class T> struct IsArithmetic
{
	enum { value = IsIntegral<T>::value || IsFloatingPoint<T>::value };
};

template<class T>
struct HasSignedValues
{
	enum { value = (typename RemoveConstVolatile<T>::type)(-1) < (typename RemoveConstVolatile<T>::type)(0) };
};

template<class T>
struct IsSigned
{
	enum { value = IsFloatingPoint<T>::value || IsIntegral<T>::value && HasSignedValues<T>::value };
};

template<class T>
struct IsUnsigned
{
	enum { value = IsArithmetic<T>::value && (IsFloatingPoint<T>::value || IsIntegral<T>::value && !HasSignedValues<T>::value) };
};

template<class T>
struct IsRegularFunction : FalseType
{};

template<class ResultType, class... ArgTypes>
struct IsRegularFunction<ResultType(ArgTypes...)> : TrueType
{};

template<class ResultType, class... ArgTypes>
struct IsRegularFunction<ResultType(ArgTypes..., ...)> : TrueType
{};

template<class T>
struct IsFunctionPointer : FalseType
{};

template<class ResultType, class... ArgTypes>
struct IsFunctionPointer<ResultType(*)(ArgTypes...)> : TrueType
{};

template<class ResultType, class... ArgTypes>
struct IsFunctionPointer<ResultType(*)(ArgTypes..., ...)> : TrueType
{};

template<bool condition, class T1, class T2>
struct TypeSelector;

template<class T1, class T2>
struct TypeSelector<true, T1, T2>
{
	typedef T1 type;
};

template<class T1, class T2>
struct TypeSelector<false, T1, T2>
{
	typedef T2 type;
};

template<class T1, class T2>
struct IsSame : FalseType
{};

template<class T>
struct IsSame<T, T> : TrueType
{};

template<bool b, class T1, class T2>
struct Conditional;

template<class T1, class T2>
struct Conditional<true, T1, T2>
{
    typedef T1 type;
};

template<class T1, class T2>
struct Conditional<false, T1, T2>
{
    typedef T2 type;
};

template<class T>
struct AddPointer
{
    typedef typename RemoveReference<T>::type* type;
};

template<class T>
struct AddConst
{
	typedef const T type;
};

template<class T>
struct RemoveExtent
{
    typedef T type;
};

template<class T, size_t t_array_size>
struct RemoveExtent<T[t_array_size]>
{
    typedef T type;
};

template<class T>
struct RemoveExtent<T[]>
{
    typedef T type;
};

template<class T>
struct RemoveAllExtents
{
    typedef T type;
};

template<class T, size_t t_array_size>
struct RemoveAllExtents<T[t_array_size]>
{
    typedef typename RemoveAllExtents<T>::type type;
};

template<class T>
struct RemoveAllExtents<T[]>
{
    typedef typename RemoveAllExtents<T>::type type;
};

template<class T>
struct ___IsVoid : FalseType {};

template<>
struct ___IsVoid<void>	: TrueType {};

template<class T>
struct IsVoid : ___IsVoid<typename RemoveConstVolatile<T>::type> {}; 

template<class From, class To>
struct IsConvertible
{
	enum { value = IsVoid<From>::value && IsVoid<To>::value || __is_convertible_to(From, To) };
};

// primary template
template<class>
struct IsFunction : FalseType
{};

// specialization for regular functions
template<class Ret, class... Args>
struct IsFunction<Ret(Args...)> : TrueType
{};

// specialization for variadic functions such as std::printf
template<class Ret, class... Args>
struct IsFunction<Ret(Args..., ...)> : TrueType
{};

template<class T>
struct Decay
{
    typedef typename RemoveReference<T>::type U;
    typedef typename Conditional<
        IsArray<U>::value,
        typename RemoveExtent<U>::type*,
        typename Conditional<
        IsFunction<U>::value,
        typename AddPointer<U>::type,
        typename RemoveConstVolatile<U>::type
        >::type
    >::type type;
};

template<class T>
typename AddRvalueReference<T>::type DeclVal();

template<class>
struct ResultOf;

template<class ResultType, class... ArgTypes>
struct ResultOf<ResultType(ArgTypes...)>
{
    typedef decltype(DeclVal<ResultType>()(DeclVal<ArgTypes>()...)) type;
};

template<class... ArgTypes>
struct ResultOf<void(ArgTypes...)>
{
    typedef void type;
};

template<class T, T t_value>
struct IntegralConstant
{
    static const T value = t_value;

    typedef T value_type;
    typedef IntegralConstant<T, t_value> type;

    operator value_type() const
    {
        return (value);
    }
};

typedef IntegralConstant<bool, true> true_type;
typedef IntegralConstant<bool, false> false_type;

template<class T>
struct Rank
    : IntegralConstant<size_t, 0>
{};

template<class T, size_t t_size>
struct Rank<T[t_size]>
    : IntegralConstant<size_t, Rank<T>::value + 1>
{};

template<class T>
struct Rank<T[]>
    : IntegralConstant<size_t, Rank<T>::value + 1>
{};

template<class T, size_t t_dim_idx>
struct _Extent_
    : IntegralConstant<size_t, 0>
{};

template<class T, size_t t_size>
struct _Extent_<T[t_size], 0>
    : IntegralConstant<size_t, t_size>
{};

template<class T, size_t t_size, size_t t_dim_idx>
struct _Extent_<T[t_size], t_dim_idx>
    : _Extent_<T, t_dim_idx - 1>
{};

template<class T, size_t t_dim_idx>
struct _Extent_<T[], t_dim_idx>
    : _Extent_<T, t_dim_idx - 1>
{};

template<class T, size_t t_dim_idx = 0>
struct Extent
    : _Extent_<T, t_dim_idx>
{};

template<class T>
constexpr bool IsLessThan(T a, T b)
{
    return a < b;
}

template<class T>
constexpr bool IsGreaterThan(T a, T b)
{
    return a > b;
}

template<class T>
constexpr bool IsLessThanOrEqualTo(T a, T b)
{
    return a <= b;
}

template<class T>
constexpr bool IsGreaterThanOrEqualTo(T a, T b)
{
    return a >= b;
}


template<class T, ENABLE_IF(IsGreaterThan(sizeof(T), sizeof(void*)))>
constexpr const T& Min(const T& a, const T& b)
{
    return a < b ? a : b;
}

template<class T, ENABLE_IF(IsGreaterThan(sizeof(T), sizeof(void*)))>
constexpr const T& Max(const T& a, const T& b)
{
    return a < b ? b : a;
}

template<class T, ENABLE_IF(IsLessThanOrEqualTo(sizeof(T), sizeof(void*)))>
constexpr T Min(T a, T b)
{
    return a < b ? a : b;
}

template<class T, ENABLE_IF(IsLessThanOrEqualTo(sizeof(T), sizeof(void*)))>
constexpr T Max(T a, T b)
{
    return a < b ? b : a;
}

template<class T, size_t t_size>
constexpr size_t GetArraySize(const T(&)[t_size])
{
    return t_size;
}

template<class T, ENABLE_IF(IsUnsigned<T>::value)>
inline constexpr auto Max()
{
    typedef typename RemoveConstVolatileReference<T>::type type;

    return type(-1);
}

template<class T>
void Swap(T& a, T& b)
{
	auto temp = Move(a);
	a = Move(b);
	b = Move(temp);
}