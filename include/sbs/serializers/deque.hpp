#ifndef SBS_SERIALIZERS_DEQUE_HPP
#define SBS_SERIALIZERS_DEQUE_HPP

#include <sbs/sbs.hpp>

#include <cstdint>
#include <deque>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct DequeSerializer {
    void operator()(Archive& ar, std::deque<Type, Allocator>& deque) const
    {
        if (ar.serializing()) {
            uint64_t size = deque.size();
            ar.archive(size);
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
void serialize(Archive& ar, std::deque<Type, Allocator>& deque)
{
    DequeSerializer<Type, TypeSerializer, Allocator>()(ar, deque);
}

}

#endif // SBS_SERIALIZERS_DEQUE_HPP