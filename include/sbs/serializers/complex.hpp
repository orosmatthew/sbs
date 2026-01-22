#ifndef SBS_SERIALIZERS_COMPLEX_HPP
#define SBS_SERIALIZERS_COMPLEX_HPP

#include <sbs/sbs.hpp>

#include <complex>

namespace sbs {

template <class Type, class TypeSerializer = DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::copyable<Type> && std::is_default_constructible_v<Type>)
struct ComplexSerializer {
    void operator()(Archive& ar, std::complex<Type>& complex) const
    {
        if (ar.serializing()) {
            Type real = complex.real();
            ar.archive<TypeSerializer>(real);
            Type imag = complex.imag();
            ar.archive<TypeSerializer>(imag);
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
void serialize(Archive& ar, std::complex<Type>& complex)
{
    ComplexSerializer<Type>()(ar, complex);
}

}

#endif // SBS_SERIALIZERS_COMPLEX_HPP