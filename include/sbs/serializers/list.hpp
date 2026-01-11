#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <list>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct ListSerializer {
    void operator()(std::list<Type, Allocator>& list, Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = list.size();
            ar.archive_copy(size);
            for (Type& element : list) {
                ar.archive<TypeSerializer>(element);
            }
        } else {
            list.clear();
            uint64_t size = 0;
            ar.archive(size);
            list.resize(size);
            for (Type& element : list) {
                ar.archive<TypeSerializer>(element);
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(std::list<Type, Allocator>& list, Archive& ar)
{
    ListSerializer<Type, TypeSerializer, Allocator>()(list, ar);
}

}