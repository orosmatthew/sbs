#ifndef SBS_SERIALIZERS_OPTIONAL_HPP
#define SBS_SERIALIZERS_OPTIONAL_HPP

#include <sbs/sbs.hpp>

#include <optional>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::is_default_constructible_v<Type>)
struct OptionalSerializer {
    void operator()(Archive& ar, std::optional<Type>& optional) const
    {
        if (ar.serializing()) {
            bool has_value = optional.has_value();
            ar.archive(has_value);
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
                optional = std::move(value);
            }
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(Archive& ar, std::optional<Type>& optional)
{
    OptionalSerializer<Type, TypeSerializer>()(ar, optional);
}

}

#endif // SBS_SERIALIZERS_OPTIONAL_HPP