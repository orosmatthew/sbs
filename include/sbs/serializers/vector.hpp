#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <vector>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct VectorSerializer {
    void operator()(std::vector<Type, Allocator>& vector, Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = vector.size();
            ar.archive_copy(size);
            for (Type& item : vector) {
                ar.archive<TypeSerializer>(item);
            }
        } else {
            vector.clear();
            uint64_t size = 0;
            ar.archive(size);
            vector.resize(size);
            for (Type& item : vector) {
                ar.archive<TypeSerializer>(item);
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(std::vector<Type, Allocator>& vector, Archive& ar)
{
    VectorSerializer<Type, TypeSerializer, Allocator>()(vector, ar);
}

}
