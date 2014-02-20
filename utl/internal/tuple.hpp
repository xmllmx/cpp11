#pragma once

template<class... Args>
struct Tuple;

template<>
struct Tuple<>
{};

template<class HeadType, class... Args>
struct Tuple<HeadType, Args...> : Tuple<Args...>
{
    HeadType       head_value;
    Tuple<Args...> tail_value;

    Tuple()
        : head_value(), tail_value()
    {}

    explicit Tuple(const HeadType& head_value, const Args... args)
        : head_value(head_value), tail_value(Tuple<Args...>(args...))
    {}
};

template<size_t t_idx, class TupleType>
struct TupleElement;

template<class HeadType, class... Args>
struct TupleElement<0, Tuple<HeadType, Args...>>
{
    typedef Tuple<HeadType, Args...> type;
};

template<size_t t_idx, class HeadType, class... Args>
struct TupleElement<t_idx, Tuple<HeadType, Args...>>
    : TupleElement<t_idx - 1, Tuple<Args...>>
{};

template<size_t t_idx, class... Args>
constexpr auto& Get(Tuple<Args...>& obj)
{
    typedef typename TupleElement<t_idx, Tuple<Args...>>::type SubTuple;

    return static_cast<SubTuple&>(obj).head_value;
}

template<size_t t_idx, class... Args>
constexpr const auto& Get(const Tuple<Args...>& obj)
{
    typedef typename TupleElement<t_idx, Tuple<Args...>>::type SubTuple;

    return static_cast<const SubTuple&>(obj).head_value;
}

template<class... Args>
Tuple<Args...> MakeTuple(Args... args)
{
    return Tuple<Args...>(args...);
}