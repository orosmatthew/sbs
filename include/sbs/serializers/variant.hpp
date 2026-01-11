#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <variant>

namespace sbs {

namespace detail {

template <class Variant, uint64_t Index = 0>
void construct_variant_at_index(Variant& variant, const uint64_t index)
{
    if constexpr (Index >= std::variant_size_v<Variant>) {
    } else if (Index == index) {
        using Type = std::variant_alternative_t<Index, Variant>;
        static_assert(std::is_default_constructible_v<Type>);
        auto value = Type();
        variant = value;
    } else {
        return construct_variant_at_index<Variant, Index + 1>(variant, index);
    }
}

}

template <class... Types>
    requires((sbs::DefaultSerializable<Types> && std::is_default_constructible_v<Types>) && ...)
struct VariantDefaultSerializer {
    void operator()(std::variant<Types...>& variant, Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t index = variant.index();
            ar.archive_copy(index);
            std::visit([&]<class T>(T& value) { ar.archive(value); }, variant);
        } else {
            uint64_t index = 0;
            ar.archive(index);
            detail::construct_variant_at_index<std::variant<Types...>>(variant, index);
            std::visit([&]<class T>(T& value) { ar.archive(value); }, variant);
        }
    }
};

template <class... Types>
    requires(sbs::DefaultSerializable<Types> && ...)
void serialize(std::variant<Types...>& variant, Archive& ar)
{
    VariantDefaultSerializer<Types...>()(variant, ar);
}

}