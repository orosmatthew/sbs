#include <sbs/sbs.hpp>

// ReSharper disable CppUnusedIncludeDirective
#include <sbs/serializers/array.hpp>
#include <sbs/serializers/bitset.hpp>
#include <sbs/serializers/chrono.hpp>
#include <sbs/serializers/complex.hpp>
#include <sbs/serializers/deque.hpp>
#include <sbs/serializers/filesystem.hpp>
#include <sbs/serializers/forward_list.hpp>
#include <sbs/serializers/list.hpp>
#include <sbs/serializers/map.hpp>
#include <sbs/serializers/memory.hpp>
#include <sbs/serializers/optional.hpp>
#include <sbs/serializers/set.hpp>
#include <sbs/serializers/string.hpp>
#include <sbs/serializers/unordered_map.hpp>
#include <sbs/serializers/unordered_set.hpp>
#include <sbs/serializers/utility.hpp>
#include <sbs/serializers/variant.hpp>
#include <sbs/serializers/vector.hpp>

#include "test.hpp"

enum class MyEnum : uint16_t { first, second, third };

namespace inner {

struct OtherStruct {
    uint64_t thing;
};

void serialize(sbs::Archive& ar, OtherStruct& o)
{
    ar.archive(o.thing);
}

}

struct SimpleStruct {
    uint8_t a;
    uint16_t b;
    uint8_t c;
    inner::OtherStruct d;
    float f;
    std::vector<uint16_t> numbers;
    std::vector<std::vector<uint8_t>> multi_nums;
    std::string str;
    std::array<uint8_t, 4> nums_array;
    MyEnum my_enum;
    std::deque<uint8_t> deque;
    std::forward_list<uint8_t> forward_list;
    std::list<uint8_t> list;
    std::set<uint8_t> set;
    std::map<std::string, uint16_t> map;
    std::multiset<uint8_t> multiset;
    std::multimap<std::string, uint16_t> multimap;
    std::unordered_set<std::string> unordered_set;
    std::unordered_map<std::string, uint16_t> unordered_map;
    std::unordered_multiset<uint8_t> unordered_multiset;
    std::unordered_multimap<std::string, uint16_t> unordered_multimap;
    std::optional<uint8_t> optional;
    std::variant<uint8_t, float> variant1;
    std::variant<uint8_t, float> variant2;
    std::pair<uint8_t, std::string> pair;
    std::tuple<int, double, float> tuple;
    std::chrono::duration<double> duration;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
    std::complex<float> complex;
    std::bitset<200> bitset;
    std::unique_ptr<int64_t> unique_ptr;
    std::unique_ptr<int64_t> unique_ptr_null;
    std::filesystem::path filesystem_path;

    void serialize(sbs::Archive& ar)
    {
        ar.archive(a);
        ar.archive(b);
        ar.archive(c);
        ar.archive(d);
        ar.archive(f);
        ar.archive(numbers);
        ar.archive(multi_nums);
        ar.archive(str);
        ar.archive(nums_array);
        ar.archive(my_enum);
        ar.archive(deque);
        ar.archive(forward_list);
        ar.archive(list);
        ar.archive(set);
        ar.archive(map);
        ar.archive(multiset);
        ar.archive(multimap);
        ar.archive(unordered_set);
        ar.archive(unordered_map);
        ar.archive(unordered_multiset);
        ar.archive(unordered_multimap);
        ar.archive(optional);
        ar.archive(variant1);
        ar.archive(variant2);
        ar.archive(pair);
        ar.archive(tuple);
        ar.archive(duration);
        ar.archive(time_point);
        ar.archive(complex);
        ar.archive(bitset);
        ar.archive(unique_ptr);
        ar.archive(unique_ptr_null);
        ar.archive(filesystem_path);
    }
};

void serialize_ints()
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
    sbs::deserialize_from_span(s_out, bytes);
    ASSERT(s_in == s_out);
}

void serialize_floats()
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
        sbs::deserialize_from_span(s_out, bytes);
        ASSERT(s_in == s_out);
    }

    test_section("negatives");
    {
        Struct s_in { .f = -2.3f, .d = -1.5 };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
        ASSERT(bytes.size() == 12);
        Struct s_out { };
        sbs::deserialize_from_span(s_out, bytes);
        ASSERT(s_in == s_out);
    }
}

void serialize_enum()
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
    sbs::deserialize_from_span(s_out, bytes);
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

void serialize(sbs::Archive& ar, FunctionSerializableStruct& s)
{
    ar.archive(s.i);
    ar.archive(s.f);
}

void serialize_function_serializable()
{
    test_case("serialize function serializable struct");

    FunctionSerializableStruct s_in { .i = 23, .f = -50000.0f };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(s_in);
    ASSERT(bytes.size() == 5);
    FunctionSerializableStruct s_out { };
    sbs::deserialize_from_span(s_out, bytes);
    ASSERT(s_in == s_out);
}

void serialize_nested_structs()
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
    sbs::deserialize_from_span(s_out, bytes);
    ASSERT(s_in == s_out);
}

int main()
{
    serialize_ints();
    serialize_floats();
    serialize_enum();
    serialize_function_serializable();
    serialize_nested_structs();

    SimpleStruct s {
        .a = 1,
        .b = 2,
        .c = 3,
        .d = { .thing = 1337 },
        .numbers = { },
        .multi_nums = { },
        .str = "Hello World!",
        .nums_array = { 10, 20, 30, 40 },
        .my_enum = MyEnum::third,
        .deque = { },
        .forward_list = { },
        .list = { },
        .set { },
        .map { },
        .multiset { },
        .multimap { },
        .unordered_set { },
        .unordered_multiset { }
    };

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

    s.forward_list.push_front(3);
    s.forward_list.push_front(6);
    s.forward_list.push_front(9);

    s.list.push_back(4);
    s.list.push_back(8);
    s.list.push_back(12);

    s.set.insert(0);
    s.set.insert(1);
    s.set.insert(2);
    s.set.insert(1);
    s.set.insert(0);

    s.map["one"] = 1;
    s.map["two"] = 4;
    s.map["three"] = 9;
    s.map["four"] = 16;

    s.multiset.insert(0);
    s.multiset.insert(1);
    s.multiset.insert(2);
    s.multiset.insert(1);
    s.multiset.insert(0);

    s.multimap.insert({ "one", 1 });
    s.multimap.insert({ "three", 3 });
    s.multimap.insert({ "two", 4 });
    s.multimap.insert({ "one", 1 });
    s.multimap.insert({ "four", 4 });
    s.multimap.insert({ "three", 9 });
    s.multimap.insert({ "two", 2 });
    s.multimap.insert({ "four", 16 });

    s.unordered_set.insert("zero");
    s.unordered_set.insert("one");
    s.unordered_set.insert("two");
    s.unordered_set.insert("one");
    s.unordered_set.insert("zero");

    s.unordered_map["one"] = 1;
    s.unordered_map["two"] = 4;
    s.unordered_map["three"] = 9;
    s.unordered_map["four"] = 16;
    s.unordered_map["two"] = 2;

    s.unordered_multiset.insert(0);
    s.unordered_multiset.insert(1);
    s.unordered_multiset.insert(2);
    s.unordered_multiset.insert(1);
    s.unordered_multiset.insert(0);

    s.unordered_multimap.insert({ "one", 1 });
    s.unordered_multimap.insert({ "three", 3 });
    s.unordered_multimap.insert({ "two", 4 });
    s.unordered_multimap.insert({ "one", 1 });
    s.unordered_multimap.insert({ "four", 4 });
    s.unordered_multimap.insert({ "three", 9 });
    s.unordered_multimap.insert({ "two", 2 });
    s.unordered_multimap.insert({ "four", 16 });

    s.optional = 8;

    s.variant1 = static_cast<uint8_t>(24);
    s.variant2 = 25.5f;

    s.pair = { 8, "eight" };
    s.tuple = { 7, 7.00000001, 7.5f };

    s.duration = std::chrono::duration<double>(37);
    s.time_point = std::chrono::steady_clock::now();

    s.complex = std::complex { 2.4f, -3.0f };

    s.bitset = { };
    s.bitset.set(5, true);
    s.bitset.set(10, true);
    s.bitset.set(100, true);
    s.bitset.set(150, true);
    s.bitset.set(198, true);

    s.unique_ptr = std::make_unique<int64_t>(1337);
    s.unique_ptr_null = nullptr;

    s.filesystem_path = "C:\\Windows\\System32";

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
