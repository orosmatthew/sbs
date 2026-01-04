#include <sbs/sbs.hpp>

#include "test.hpp"

#include <array>
#include <cstdint>
#include <deque>

enum class MyEnum : uint16_t { first, second, third };

template <
    class Element,
    class ElementSerialize = sbs::DefaultSerialize<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serialize<ElementSerialize, Element>)
struct VectorSerialize {
    void operator()(std::vector<Element, Allocator>& vector, const sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            auto size = vector.size();
            ar.archive(size);
            for (Element& item : vector) {
                ar.archive<ElementSerialize>(item);
            }
        }
        if (ar.deserializing()) {
            vector.clear();
            uint64_t size = 0;
            ar.archive(size);
            vector.resize(size);
            for (Element& item : vector) {
                ar.archive<ElementSerialize>(item);
            }
        }
    }
};

template <
    class Char,
    class CharSerialize = sbs::DefaultSerialize<Char>,
    class Traits = std::char_traits<Char>,
    class Allocator = std::allocator<Char>>
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

template <class Element, std::size_t Size, class ElementSerialize = sbs::DefaultSerialize<Element>>
    requires(sbs::Serialize<ElementSerialize, Element>)
struct ArraySerialize {
    void operator()(std::array<Element, Size>& array, const sbs::Archive& ar) const
    {
        for (Element& element : array) {
            ar.archive<ElementSerialize>(element);
        }
    }
};

template <
    class Element,
    class ElementSerialize = sbs::DefaultSerialize<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serialize<ElementSerialize, Element>)
struct DequeSerialize {
    void operator()(std::deque<Element, Allocator>& deque, const sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            uint64_t size = deque.size();
            ar.archive(size);
            for (Element& element : deque) {
                ar.archive<ElementSerialize>(element);
            }
        } else if (ar.deserializing()) {
            deque.clear();
            uint64_t size = 0;
            ar.archive(size);
            deque.resize(size);
            for (Element& element : deque) {
                ar.archive<ElementSerialize>(element);
            }
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
    std::deque<uint8_t> deque;

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
        ar.archive<DequeSerialize<uint8_t>>(deque);
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
                     .my_enum = MyEnum::third,
                     .deque = { } };

    s.numbers.push_back(2);
    s.numbers.push_back(4);
    s.numbers.push_back(6);
    s.numbers.push_back(8);

    s.multi_nums.push_back({ { 1, 2, 3 } });
    s.multi_nums.push_back({ { 4, 5, 6 } });
    s.multi_nums.push_back({ { 7, 8, 9 } });
    s.multi_nums.push_back({ { 10, 11, 12 } });

    s.deque.push_back(5);
    s.deque.push_back(4);
    s.deque.push_back(3);
    s.deque.push_back(2);
    s.deque.push_back(1);

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
