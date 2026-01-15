#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <vector>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct VectorSerializer {
    void operator()(Archive& ar, std::vector<Type, Allocator>& vector) const
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
void serialize(Archive& ar, std::vector<Type, Allocator>& vector)
{
    VectorSerializer<Type, TypeSerializer, Allocator>()(ar, vector);
}

}
