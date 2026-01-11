#pragma once

#include <sbs/sbs.hpp>

#include <array>

namespace sbs {

template <class Type, std::size_t size, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
struct ArraySerializer {
    void operator()(std::array<Type, size>& array, Archive& ar) const
    {
        for (Type& element : array) {
            ar.archive<TypeSerializer>(element);
        }
    }
};

template <class Type, std::size_t size, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(std::array<Type, size>& array, Archive& ar)
{
    ArraySerializer<Type, size, TypeSerializer>()(array, ar);
}

}