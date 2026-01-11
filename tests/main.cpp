#include <sbs/sbs.hpp>

// ReSharper disable CppUnusedIncludeDirective
#include <sbs/serializers/array.hpp>
#include <sbs/serializers/bitset.hpp>
#include <sbs/serializers/chrono.hpp>
#include <sbs/serializers/complex.hpp>
#include <sbs/serializers/deque.hpp>
#include <sbs/serializers/forward_list.hpp>
#include <sbs/serializers/list.hpp>
#include <sbs/serializers/map.hpp>
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

struct OtherStruct {
    uint64_t thing;
};

void serialize(sbs::Archive& ar, OtherStruct& o)
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

    void serialize(sbs::Archive& ar)
    {
        ar.archive(a);
        ar.archive(b);
        ar.archive(c);
        ar.archive(d);
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
    }
};

int main()
{
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

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
