#pragma once

#include <sbs/sbs.hpp>

#include <optional>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct OptionalSerializer {
    void operator()(std::optional<Type>& optional, Archive& ar) const
    {
        if (ar.serializing()) {
            const bool has_value = optional.has_value();
            ar.archive_copy(has_value);
            if (has_value) {
                ar.archive<TypeSerializer>(optional.value());
            }
        } else {
            optional.reset();
            bool has_value = false;
            ar.archive(has_value);
            if (has_value) {
                auto value = Type();
                ar.archive<TypeSerializer>(value);
                optional = value;
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(std::optional<Type>& optional, Archive& ar)
{
    OptionalSerializer<Type, TypeSerializer>()(optional, ar);
}

}