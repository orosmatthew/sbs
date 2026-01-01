#include <sbs/sbs.hpp>

#include "test.hpp"

#include <cstdint>

struct OtherStruct {
    uint64_t thing;
};

void serialize(OtherStruct& o, sbs::Archive& ar)
{
    ar.value64(o.thing);
}

struct SimpleStruct {
    uint8_t a;
    uint16_t b;
    uint8_t c;
    OtherStruct d;

    void serialize(sbs::Archive& ar)
    {
        ar.value8(a);
        ar.value16(b);
        ar.value8(c);
        ar.object(d);
    }
};

int main()
{
    SimpleStruct s { .a = 1, .b = 2, .c = 3, .d = { .thing = 1337 } };

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
