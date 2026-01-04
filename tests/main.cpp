#include <sbs/sbs.hpp>

#include "test.hpp"

#include <cstdint>

template <typename Type, typename SerializeElement = sbs::DefaultSerialize<Type>>
    requires(sbs::Serialize<SerializeElement, Type>)
struct VectorSerialize {
    void operator()(std::vector<Type>& vector, const sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            uint64_t size = vector.size();
            ar.archive(size);
            for (Type& item : vector) {
                ar.archive<SerializeElement>(item);
            }
        }
        if (ar.deserializing()) {
            vector.clear();
            uint64_t size;
            ar.archive(size);
            vector.resize(size);
            for (Type& item : vector) {
                ar.archive<SerializeElement>(item);
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

    void serialize(const sbs::Archive& ar)
    {
        ar.archive(a);
        ar.archive(b);
        ar.archive(c);
        ar.archive(d);
        ar.archive<VectorSerialize<uint16_t>>(numbers);
        ar.archive<VectorSerialize<std::vector<uint8_t>, VectorSerialize<uint8_t>>>(multi_nums);
    }
};

int main()
{
    SimpleStruct s { .a = 1, .b = 2, .c = 3, .d = { .thing = 1337 }, .numbers = { } };

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
