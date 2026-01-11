#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <forward_list>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct ForwardListSerializer {
    void operator()(std::forward_list<Type, Allocator>& forward_list, Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = std::distance(forward_list.begin(), forward_list.end());
            ar.archive_copy(size);
            for (Type& element : forward_list) {
                ar.archive<TypeSerializer>(element);
            }
        } else {
            forward_list.clear();
            uint64_t size = 0;
            ar.archive(size);
            forward_list.resize(size);
            for (Type& element : forward_list) {
                ar.archive<TypeSerializer>(element);
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(std::forward_list<Type, Allocator>& forward_list, Archive& ar)
{
    ForwardListSerializer<Type, TypeSerializer, Allocator>()(forward_list, ar);
}

}