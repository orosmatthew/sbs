#pragma once

#include <sbs/sbs.hpp>

#include <memory>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Deleter = std::default_delete<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct UniquePtrSerializer {
    void operator()(Archive& ar, std::unique_ptr<Type, Deleter>& unique_ptr) const
    {
        if (ar.serializing()) {
            const bool has_value = unique_ptr != nullptr;
            ar.archive_copy(has_value);
            if (has_value) {
                ar.archive<TypeSerializer>(*unique_ptr);
            }
        } else {
            bool has_value = false;
            ar.archive(has_value);
            if (has_value) {
                auto value = std::unique_ptr<Type, Deleter>(new Type());
                ar.archive(*value);
                unique_ptr = std::move(value);
            } else {
                unique_ptr.reset();
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>, class Deleter = std::default_delete<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(Archive& ar, std::unique_ptr<Type, Deleter>& unique_ptr)
{
    UniquePtrSerializer<Type, TypeSerializer, Deleter>()(ar, unique_ptr);
}

}