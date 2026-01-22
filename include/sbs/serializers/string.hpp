#ifndef SBS_SERIALIZERS_STRING_HPP
#define SBS_SERIALIZERS_STRING_HPP

#include <sbs/sbs.hpp>

#include <cstdint>
#include <string>

namespace sbs {

template <
    class CharType,
    class CharTypeSerializer = DefaultSerializer<CharType>,
    class Traits = std::char_traits<CharType>,
    class Allocator = std::allocator<CharType>>
    requires(sbs::Serializer<CharTypeSerializer, CharType> && std::is_default_constructible_v<CharType>)
struct BasicStringSerializer {
    void operator()(Archive& ar, std::basic_string<CharType, Traits, Allocator>& string) const
    {
        if (ar.serializing()) {
            uint64_t size = string.size();
            ar.archive(size);
            for (auto& element : string) {
                ar.archive<CharTypeSerializer>(element);
            }
        } else {
            string.clear();
            uint64_t size = 0;
            ar.archive(size);
            string.resize(size);
            for (auto& element : string) {
                ar.archive<CharTypeSerializer>(element);
            }
        }
    }
};

using StringSerializer = BasicStringSerializer<char>;

template <
    class CharType,
    class CharTypeSerializer = DefaultSerializer<CharType>,
    class Traits = std::char_traits<CharType>,
    class Allocator = std::allocator<CharType>>
    requires(sbs::Serializer<CharTypeSerializer, CharType>)
void serialize(Archive& ar, std::basic_string<CharType, Traits, Allocator>& basic_string)
{
    BasicStringSerializer<CharType, CharTypeSerializer, Traits, Allocator>()(ar, basic_string);
}

}

#endif // SBS_SERIALIZERS_STRING_HPP
