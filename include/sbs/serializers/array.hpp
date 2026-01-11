#pragma once

#include <sbs/sbs.hpp>

#include <array>

namespace sbs {

template <class Type, std::size_t size, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
struct ArraySerializer {
    void operator()(Archive& ar, std::array<Type, size>& array) const
    {
        for (Type& element : array) {
            ar.archive<TypeSerializer>(element);
        }
    }
};

template <class Type, std::size_t size, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(Archive& ar, std::array<Type, size>& array)
{
    ArraySerializer<Type, size, TypeSerializer>()(ar, array);
}

}