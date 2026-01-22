#ifndef SBS_SERIALIZERS_UTILITY_HPP
#define SBS_SERIALIZERS_UTILITY_HPP

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
    void operator()(Archive& ar, std::pair<First, Second>& pair) const
    {
        ar.archive<FirstSerializer>(pair.first);
        ar.archive<SecondSerializer>(pair.second);
    }
};

namespace detail {

template <class Tuple, std::size_t Index = 0>
void serialize_tuple(Archive& ar, Tuple& tuple)
{
    if constexpr (Index < std::tuple_size_v<Tuple>) {
        ar.archive(std::get<Index>(tuple));
        serialize_tuple<Tuple, Index + 1>(ar, tuple);
    }
}

}

template <class... Types>
    requires(sbs::DefaultSerializable<Types> && ...)
struct TupleDefaultSerializer {
    void operator()(Archive& ar, std::tuple<Types...>& tuple) const
    {
        detail::serialize_tuple(ar, tuple);
    }
};

template <
    class First,
    class Second,
    class FirstSerializer = DefaultSerializer<First>,
    class SecondSerializer = DefaultSerializer<Second>>
    requires(sbs::Serializer<FirstSerializer, First> && sbs::Serializer<SecondSerializer, Second>)
void serialize(Archive& ar, std::pair<First, Second>& pair)
{
    PairSerializer<First, Second, FirstSerializer, SecondSerializer>()(ar, pair);
}

template <class... Types>
    requires(sbs::DefaultSerializable<Types> && ...)
void serialize(Archive& ar, std::tuple<Types...>& tuple)
{
    TupleDefaultSerializer<Types...>()(ar, tuple);
}

}

#endif // SBS_SERIALIZERS_UTILITY_HPP