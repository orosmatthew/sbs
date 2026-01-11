#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <deque>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct DequeSerializer {
    void operator()(std::deque<Type, Allocator>& deque, Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = deque.size();
            ar.archive_copy(size);
            for (Type& element : deque) {
                ar.archive<TypeSerializer>(element);
            }
        } else {
            deque.clear();
            uint64_t size = 0;
            ar.archive(size);
            deque.resize(size);
            for (Type& element : deque) {
                ar.archive<TypeSerializer>(element);
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
void serialize(std::deque<Type, Allocator>& deque, Archive& ar)
{
    DequeSerializer<Type, TypeSerializer, Allocator>()(deque, ar);
}

}