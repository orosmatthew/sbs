#ifndef SBS_SERIALIZERS_LIST_HPP
#define SBS_SERIALIZERS_LIST_HPP

#include <sbs/sbs.hpp>

#include <cstdint>
#include <list>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Allocator = std::allocator<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct ListSerializer {
    void operator()(Archive& ar, std::list<Type, Allocator>& list) const
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
void serialize(Archive& ar, std::list<Type, Allocator>& list)
{
    ListSerializer<Type, TypeSerializer, Allocator>()(ar, list);
}

}

#endif // SBS_SERIALIZERS_LIST_HPP