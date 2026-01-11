#pragma once

#include <sbs/sbs.hpp>

#include <complex>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::copyable<Type> && std::is_default_constructible_v<Type>)
struct ComplexSerializer {
    void operator()(std::complex<Type>& complex, Archive& ar) const
    {
        if (ar.serializing()) {
            ar.archive_copy<TypeSerializer>(complex.real());
            ar.archive_copy<TypeSerializer>(complex.imag());
        } else {
            auto real = Type();
            ar.archive<TypeSerializer>(real);
            complex.real(real);
            auto imag = Type();
            ar.archive<TypeSerializer>(imag);
            complex.imag(imag);
        }
    }
};

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type>)
void serialize(std::complex<Type>& complex, Archive& ar)
{
    ComplexSerializer<Type>()(complex, ar);
}

}