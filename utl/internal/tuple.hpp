#pragma once

template <class... Args>
class Tuple;

template <>
class Tuple<>
{};

template<class HeadType, class... TailType>
class Tuple<HeadType, TailType...>
    : Tuple<TailType...>
{
public:
    HeadType value;
};

template <size_t t_idx, class T>
struct TupleElement;

template <size_t t_idx, class HeadType, class... TailType>
struct TupleElement<t_idx, Tuple<HeadType, TailType...>>
    : TupleElement<t_idx - 1, Tuple<TailType...>>
{};

template <class HeadType, class... TailType>
struct TupleElement<0, Tuple<HeadType, TailType...>>
{
    typedef Tuple<HeadType, TailType...> type;
};

template<size_t t_idx, class... Args>
constexpr auto& Get(Tuple<Args...>& obj)
{
    typedef typename TupleElement<t_idx, Tuple<Args...>>::type type;

    return ((type&)(obj)).value;
}

template<size_t t_idx, class... Args>
constexpr const auto& Get(const Tuple<Args...>& obj)
{
    typedef typename TupleElement<t_idx, Tuple<Args...>>::type type;

    return ((const type&)(obj)).value;
}