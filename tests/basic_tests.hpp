#pragma once

// ReSharper disable CppUnusedIncludeDirective

#include "test.hpp"

#include <sbs/sbs.hpp>

#include <array>
#include <bit>
#include <cstdint>
#include <vector>

inline void assert_bytes_equal(const std::span<const std::byte> bytes, const std::initializer_list<uint8_t>& expected)
{
    ASSERT(bytes.size() == expected.size());
    for (size_t i = 0; i < bytes.size(); ++i) {
        ASSERT(bytes[i] == std::byte { *(expected.begin() + i) });
    }
}

inline void test_endianness()
{
    test_case("test endianness");

    uint8_t uint8 = 37;
    uint16_t uint16 = 6123;
    uint32_t uint32 = 2124521023;
    uint64_t uint64 = 13244784333009251345ULL;

    test_section("uint8 little endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint8, std::endian::little);
        assert_bytes_equal(b, { 0x25 });
        uint8_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::little);
        ASSERT(r == uint8);
    }

    test_section("uint16 little endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint16, std::endian::little);
        assert_bytes_equal(b, { 0xEB, 0x17 });
        uint16_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::little);
        ASSERT(r == uint16);
    }

    test_section("uint32 little endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint32, std::endian::little);
        assert_bytes_equal(b, { 0x3F, 0x9E, 0xA1, 0x7E });
        uint32_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::little);
        ASSERT(r == uint32);
    }

    test_section("uint64 little endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint64, std::endian::little);
        assert_bytes_equal(b, { 0x11, 0xEC, 0xAC, 0x4F, 0x2D, 0xED, 0xCE, 0xB7 });
        uint64_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::little);
        ASSERT(r == uint64);
    }

    test_section("uint8 big endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint8, std::endian::big);
        assert_bytes_equal(b, { 0x25 });
        uint8_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::big);
        ASSERT(r == uint8);
    }

    test_section("uint16 big endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint16, std::endian::big);
        assert_bytes_equal(b, { 0x17, 0xEB });
        uint16_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::big);
        ASSERT(r == uint16);
    }

    test_section("uint32 big endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint32, std::endian::big);
        assert_bytes_equal(b, { 0x7E, 0xA1, 0x9E, 0x3F });
        uint32_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::big);
        ASSERT(r == uint32);
    }

    test_section("uint64 big endian");
    {
        const std::vector<std::byte> b = sbs::serialize_to_vector(uint64, std::endian::big);
        assert_bytes_equal(b, { 0xB7, 0xCE, 0xED, 0x2D, 0x4F, 0xAC, 0xEC, 0x11 });
        uint64_t r = 0;
        sbs::deserialize_from_span(b, r, std::endian::big);
        ASSERT(r == uint64);
    }

    test_section("float little endian");
    {
        float f = 12.34f;
        const std::vector<std::byte> b = sbs::serialize_to_vector(f, std::endian::little);
        assert_bytes_equal(b, { 0xA4, 0x70, 0x45, 0x41 });
        float r = 0;
        sbs::deserialize_from_span(b, r, std::endian::little);
        ASSERT(r == f);
    }

    test_section("float big endian");
    {
        float f = 12.34f;
        const std::vector<std::byte> b = sbs::serialize_to_vector(f, std::endian::big);
        assert_bytes_equal(b, { 0x41, 0x45, 0x70, 0xA4 });
        float r = 0;
        sbs::deserialize_from_span(b, r, std::endian::big);
        ASSERT(r == f);
    }

    test_section("double little endian");
    {
        double d = 1234.5678;
        const std::vector<std::byte> b = sbs::serialize_to_vector(d, std::endian::little);
        assert_bytes_equal(b, { 0xAD, 0xFA, 0x5C, 0x6D, 0x45, 0x4A, 0x93, 0x40 });
        double r = 0;
        sbs::deserialize_from_span(b, r, std::endian::little);
        ASSERT(r == d);
    }

    test_section("double big endian");
    {
        double d = 1234.5678;
        const std::vector<std::byte> b = sbs::serialize_to_vector(d, std::endian::big);
        assert_bytes_equal(b, { 0x40, 0x93, 0x4A, 0x45, 0x6D, 0x5C, 0xFA, 0xAD });
        double r = 0;
        sbs::deserialize_from_span(b, r, std::endian::big);
        ASSERT(r == d);
    }
}

inline void serialize_ints()
{
    test_case("serialize ints");

    struct Struct {
        uint8_t uint8;
        uint16_t uint16;
        uint32_t uint32;
        uint64_t uint64;
        int8_t int8;
        int16_t int16;
        int32_t int32;
        int64_t int64;

        void serialize(sbs::Archive& ar)
        {
            ar.archive(uint8);
            ar.archive(uint16);
            ar.archive(uint32);
            ar.archive(uint64);
            ar.archive(int8);
            ar.archive(int16);
            ar.archive(int32);
            ar.archive(int64);
        }

        bool operator==(const Struct& other) const
        {
            return uint8 == other.uint8 && uint16 == other.uint16 && uint32 == other.uint32 && uint64 == other.uint64
                && int8 == other.int8 && int16 == other.int16 && int32 == other.int32 && int64 == other.int64;
        }
    };

    Struct s_in { .uint8 = 2,
                  .uint16 = 60000,
                  .uint32 = 4000000000,
                  .uint64 = 100000000000000ULL,
                  .int8 = -2,
                  .int16 = -30000,
                  .int32 = -1000000000,
                  .int64 = -100000000000000LL };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
    ASSERT(bytes.size() == 30);
    Struct s_out { };
    sbs::deserialize_from_span(bytes, s_out);
    ASSERT(s_in == s_out);
}

inline void serialize_floats()
{
    test_case("serialize floats");

    struct Struct {
        float f;
        double d;

        void serialize(sbs::Archive& ar)
        {
            ar.archive(f);
            ar.archive(d);
        }

        bool operator==(const Struct& other) const
        {
            return f == other.f && d == other.d;
        }
    };

    test_section("positives");
    {
        Struct s_in { .f = 1.5f, .d = 2.3 };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
        ASSERT(bytes.size() == 12);
        Struct s_out { };
        sbs::deserialize_from_span(bytes, s_out);
        ASSERT(s_in == s_out);
    }

    test_section("negatives");
    {
        Struct s_in { .f = -2.3f, .d = -1.5 };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
        ASSERT(bytes.size() == 12);
        Struct s_out { };
        sbs::deserialize_from_span(bytes, s_out);
        ASSERT(s_in == s_out);
    }
}

inline void serialize_enum()
{
    test_case("serialize enum");

    enum class MyEnum : uint16_t { first, second, third, big = 60000 };

    struct Struct {
        MyEnum e1;
        MyEnum e2;
        MyEnum e3;
        MyEnum e4;

        void serialize(sbs::Archive& ar)
        {
            ar.archive(e1);
            ar.archive(e2);
            ar.archive(e3);
            ar.archive(e4);
        }

        bool operator==(const Struct& other) const
        {
            return e1 == other.e1 && e2 == other.e2 && e3 == other.e3 && e4 == other.e4;
        }
    };

    Struct s_in { .e1 = MyEnum::first, .e2 = MyEnum::second, .e3 = MyEnum::third, .e4 = MyEnum::big };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
    ASSERT(bytes.size() == 8);
    Struct s_out { };
    sbs::deserialize_from_span(bytes, s_out);
    ASSERT(s_in == s_out);
}

struct FunctionSerializableStruct {
    uint8_t i;
    float f;

    bool operator==(const FunctionSerializableStruct& other) const
    {
        return i == other.i && f == other.f;
    }
};

inline void serialize(sbs::Archive& ar, FunctionSerializableStruct& s)
{
    ar.archive(s.i);
    ar.archive(s.f);
}

inline void serialize_function_serializable()
{
    test_case("serialize function serializable struct");

    FunctionSerializableStruct s_in { .i = 23, .f = -50000.0f };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
    ASSERT(bytes.size() == 5);
    FunctionSerializableStruct s_out { };
    sbs::deserialize_from_span(bytes, s_out);
    ASSERT(s_in == s_out);
}

inline void serialize_nested_structs()
{
    test_case("serialize nested structs");

    struct StructInner {
        uint64_t i1;
        int32_t i2;

        void serialize(sbs::Archive& ar)
        {
            ar.archive(i1);
            ar.archive(i2);
        }

        bool operator==(const StructInner& other) const
        {
            return i1 == other.i1 && i2 == other.i2;
        }
    };

    struct StructOuter {
        uint16_t i1;
        double d;
        StructInner inner;
        int64_t i2;
        float f;

        void serialize(sbs::Archive& ar)
        {
            ar.archive(i1);
            ar.archive(d);
            ar.archive(inner);
            ar.archive(i2);
            ar.archive(f);
        }

        bool operator==(const StructOuter& other) const
        {
            return i1 == other.i1 && d == other.d && inner == other.inner && i2 == other.i2 && f == other.f;
        }
    };

    StructOuter s_in { .i1 = 1234,
                       .d = -70273.0,
                       .inner = { .i1 = 3828362829372893ULL, .i2 = -23 },
                       .i2 = -3828362829372893LL,
                       .f = 93.0f };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
    ASSERT(bytes.size() == 34);
    StructOuter s_out { };
    sbs::deserialize_from_span(bytes, s_out);
    ASSERT(s_in == s_out);
}