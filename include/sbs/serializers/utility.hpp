#pragma once

#include <sbs/sbs.hpp>

#include <utility>

namespace sbs {

template <
    class First,
    class Second,
    class FirstSerializer = DefaultSerializer<First>,
    class SecondSerializer = DefaultSerializer<Second>>
    requires(sbs::Serializer<FirstSerializer, First> && sbs::Serializer<SecondSerializer, Second>)
struct PairSerializer {
    void operator()(std::pair<First, Second>& pair, Archive& ar) const
    {
        ar.archive<FirstSerializer>(pair.first);
        ar.archive<SecondSerializer>(pair.second);
    }
};

namespace detail {

template <class Tuple, std::size_t Index = 0>
void serialize_tuple(Tuple& tuple, Archive& ar)
{
    if constexpr (Index < std::tuple_size_v<Tuple>) {
        ar.archive(std::get<Index>(tuple));
        serialize_tuple<Tuple, Index + 1>(tuple, ar);
    }
}

}

template <class... Types>
    requires(sbs::DefaultSerializable<Types> && ...)
struct TupleDefaultSerializer {
    void operator()(std::tuple<Types...>& tuple, Archive& ar) const
    {
        detail::serialize_tuple(tuple, ar);
    }
};

template <
    class First,
    class Second,
    class FirstSerializer = DefaultSerializer<First>,
    class SecondSerializer = DefaultSerializer<Second>>
    requires(sbs::Serializer<FirstSerializer, First> && sbs::Serializer<SecondSerializer, Second>)
void serialize(std::pair<First, Second>& pair, Archive& ar)
{
    PairSerializer<First, Second, FirstSerializer, SecondSerializer>()(pair, ar);
}

template <class... Types>
    requires(sbs::DefaultSerializable<Types> && ...)
void serialize(std::tuple<Types...>& tuple, Archive& ar)
{
    TupleDefaultSerializer<Types...>()(tuple, ar);
}

}