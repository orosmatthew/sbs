#include <sbs/sbs.hpp>

#include "test.hpp"

#include <array>
#include <cstdint>

enum class MyEnum : uint16_t { first, second, third };

template <typename Element, typename SerializeElement = sbs::DefaultSerialize<Element>>
    requires(sbs::Serialize<SerializeElement, Element>)
struct VectorSerialize {
    void operator()(std::vector<Element>& vector, const sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            auto size = vector.size();
            ar.archive(size);
            for (Element& item : vector) {
                ar.archive<SerializeElement>(item);
            }
        }
        if (ar.deserializing()) {
            vector.clear();
            uint64_t size = 0;
            ar.archive(size);
            vector.resize(size);
            for (Element& item : vector) {
                ar.archive<SerializeElement>(item);
            }
        }
    }
};

template <
    typename Char,
    typename CharSerialize = sbs::DefaultSerialize<Char>,
    typename Traits = std::char_traits<Char>,
    typename Allocator = std::allocator<Char>>
    requires(sbs::Serialize<CharSerialize, Char>)
struct BasicStringSerialize {
    void operator()(std::basic_string<Char, Traits, Allocator>& string, const sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            auto size = string.size();
            ar.archive(size);
            for (auto& element : string) {
                ar.archive<CharSerialize>(element);
            }
        }
        if (ar.deserializing()) {
            string.clear();
            uint64_t size = 0;
            ar.archive(size);
            string.resize(size);
            for (auto& element : string) {
                ar.archive<CharSerialize>(element);
            }
        }
    }
};

using StringSerialize = BasicStringSerialize<char>;

template <typename Element, std::size_t Size, typename ElementSerialize = sbs::DefaultSerialize<Element>>
    requires(sbs::Serialize<ElementSerialize, Element>)
struct ArraySerialize {
    void operator()(std::array<Element, Size>& array, const sbs::Archive& ar) const
    {
        for (Element& element : array) {
            ar.archive<ElementSerialize>(element);
        }
    }
};

struct OtherStruct {
    uint64_t thing;
};

void serialize(OtherStruct& o, const sbs::Archive& ar)
{
    ar.archive(o.thing);
}

struct SimpleStruct {
    uint8_t a;
    uint16_t b;
    uint8_t c;
    OtherStruct d;
    std::vector<uint16_t> numbers;
    std::vector<std::vector<uint8_t>> multi_nums;
    std::string str;
    std::array<uint8_t, 4> nums_array;
    MyEnum my_enum;

    void serialize(const sbs::Archive& ar)
    {
        ar.archive(a);
        ar.archive(b);
        ar.archive(c);
        ar.archive(d);
        ar.archive<VectorSerialize<uint16_t>>(numbers);
        ar.archive<VectorSerialize<std::vector<uint8_t>, VectorSerialize<uint8_t>>>(multi_nums);
        ar.archive<StringSerialize>(str);
        ar.archive<ArraySerialize<uint8_t, 4>>(nums_array);
        ar.archive(my_enum);
    }
};

int main()
{
    SimpleStruct s { .a = 1,
                     .b = 2,
                     .c = 3,
                     .d = { .thing = 1337 },
                     .numbers = { },
                     .multi_nums = { },
                     .str = "Hello World!",
                     .nums_array = { 10, 20, 30, 40 },
                     .my_enum = MyEnum::third };

    s.numbers.push_back(2);
    s.numbers.push_back(4);
    s.numbers.push_back(6);
    s.numbers.push_back(8);

    s.multi_nums.push_back({ { 1, 2, 3 } });
    s.multi_nums.push_back({ { 4, 5, 6 } });
    s.multi_nums.push_back({ { 7, 8, 9 } });
    s.multi_nums.push_back({ { 10, 11, 12 } });

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
