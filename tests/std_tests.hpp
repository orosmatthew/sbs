#pragma once

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

inline void serialize_array()
{
    test_case("serialize <array>");

    test_section("std::array");
    {
        std::array<uint8_t, 4> array_in { 10, 20, 30, 40 };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(array_in);
        std::array<uint8_t, 4> array_out { };
        sbs::deserialize_from_span(bytes, array_out);
        ASSERT(array_in == array_out);
    }
}

inline void serialize_bitset()
{
    test_case("serialize <bitset>");

    test_section("std::bitset");
    {
        std::bitset<200> bitset_in { };
        bitset_in.set(5, true);
        bitset_in.set(10, true);
        bitset_in.set(100, true);
        bitset_in.set(150, true);
        bitset_in.set(198, true);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(bitset_in);
        std::bitset<200> bitset_out { };
        sbs::deserialize_from_span(bytes, bitset_out);
        ASSERT(bitset_in == bitset_out);
    }
}

inline void serialize_chrono()
{
    test_case("serialize <chrono>");

    test_section("std::chrono::duration");
    {
        std::chrono::duration<double> duration_in { 37 };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(duration_in);
        std::chrono::duration<double> duration_out { };
        sbs::deserialize_from_span(bytes, duration_out);
        ASSERT(duration_in == duration_out);
    }

    test_section("std::chrono::time_point");
    {
        std::chrono::time_point<std::chrono::steady_clock> time_point_in = std::chrono::steady_clock::now();
        std::vector<std::byte> bytes = sbs::serialize_to_vector(time_point_in);
        std::chrono::time_point<std::chrono::steady_clock> time_point_out { };
        sbs::deserialize_from_span(bytes, time_point_out);
        ASSERT(time_point_in == time_point_out);
    }
}

inline void serialize_complex()
{
    test_case("serialize <complex>");

    test_section("std::complex");
    {
        std::complex complex_in { 2.4f, -3.0f };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(complex_in);
        std::complex<float> complex_out { };
        sbs::deserialize_from_span(bytes, complex_out);
        ASSERT(complex_in == complex_out);
    }
}

inline void serialize_deque()
{
    test_case("serialize <deque>");

    test_section("std::deque");
    {
        std::deque<uint8_t> deque_in { };
        deque_in.push_back(5);
        deque_in.push_back(4);
        deque_in.push_back(3);
        deque_in.push_back(2);
        deque_in.push_back(1);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(deque_in);
        std::deque<uint8_t> deque_out { };
        sbs::deserialize_from_span(bytes, deque_out);
        ASSERT(deque_in == deque_out);
    }
}

inline void serialize_filesystem()
{
    test_case("serialize <filesystem>");

    test_section("std::filesystem::path");
    {
        std::filesystem::path path_in { R"(C:\Windows\System32\notepad.exe)" };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(path_in);
        std::filesystem::path path_out { };
        sbs::deserialize_from_span(bytes, path_out);
        ASSERT(path_in == path_out);
    }
}

inline void serialize_forward_list()
{
    test_case("serialize <forward_list>");

    test_section("std::forward_list");
    {
        std::forward_list<uint8_t> list_in { };
        list_in.push_front(3);
        list_in.push_front(6);
        list_in.push_front(9);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(list_in);
        std::forward_list<uint8_t> list_out { };
        sbs::deserialize_from_span(bytes, list_out);
        ASSERT(list_in == list_out);
    }
}

inline void serialize_list()
{
    test_case("serialize <list>");

    test_section("std::list");
    {
        std::list<uint8_t> list_in { };
        list_in.push_back(4);
        list_in.push_back(8);
        list_in.push_back(12);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(list_in);
        std::list<uint8_t> list_out { };
        sbs::deserialize_from_span(bytes, list_out);
        ASSERT(list_in == list_out);
    }
}

inline void serialize_map()
{
    test_case("serialize <map>");

    test_section("std::map");
    {
        std::map<std::string, uint16_t> map_in { };
        map_in.insert({ "one", 1 });
        map_in.insert({ "two", 4 });
        map_in.insert({ "three", 9 });
        map_in.insert({ "four", 16 });
        std::vector<std::byte> bytes = sbs::serialize_to_vector(map_in);
        std::map<std::string, uint16_t> map_out { };
        sbs::deserialize_from_span(bytes, map_out);
        ASSERT(map_in == map_out);
    }

    test_section("std::multimap");
    {
        std::multimap<std::string, uint16_t> multimap_in { };
        multimap_in.insert({ "one", 1 });
        multimap_in.insert({ "three", 3 });
        multimap_in.insert({ "two", 4 });
        multimap_in.insert({ "one", 1 });
        multimap_in.insert({ "four", 4 });
        multimap_in.insert({ "three", 9 });
        multimap_in.insert({ "two", 2 });
        multimap_in.insert({ "four", 16 });
        std::vector<std::byte> bytes = sbs::serialize_to_vector(multimap_in);
        std::multimap<std::string, uint16_t> multimap_out { };
        sbs::deserialize_from_span(bytes, multimap_out);
        ASSERT(multimap_in == multimap_out);
    }
}

inline void serialize_memory()
{
    test_case("serialize <memory>");

    test_section("std::unique_ptr");
    {
        {
            auto ptr_in = std::make_unique<int64_t>(1337);
            std::vector<std::byte> bytes = sbs::serialize_to_vector(ptr_in);
            std::unique_ptr<int64_t> ptr_out { };
            sbs::deserialize_from_span(bytes, ptr_out);
            ASSERT(ptr_in != nullptr && ptr_out != nullptr && *ptr_in == *ptr_out);
        }
        {
            std::unique_ptr<int64_t> ptr_in = nullptr;
            std::vector<std::byte> bytes = sbs::serialize_to_vector(ptr_in);
            std::unique_ptr<int64_t> ptr_out { };
            sbs::deserialize_from_span(bytes, ptr_out);
            ASSERT(ptr_in == nullptr && ptr_in == ptr_out);
        }
    }
}

inline void serialize_optional()
{
    test_case("serialize <optional>");

    test_section("std::optional");
    {
        {
            std::optional<uint8_t> opt_in = 8;
            std::vector<std::byte> bytes = sbs::serialize_to_vector(opt_in);
            std::optional<uint8_t> opt_out { };
            sbs::deserialize_from_span(bytes, opt_out);
            ASSERT(opt_in.has_value() && opt_out.has_value() && *opt_in == *opt_out);
        }
        {
            std::optional<uint8_t> opt_in = std::nullopt;
            std::vector<std::byte> bytes = sbs::serialize_to_vector(opt_in);
            std::optional<uint8_t> opt_out { };
            sbs::deserialize_from_span(bytes, opt_out);
            ASSERT(!opt_in.has_value() && !opt_out.has_value());
        }
    }
}

inline void serialize_set()
{
    test_case("serialize <set>");

    test_section("std::set");
    {
        std::set<uint8_t> set_in { };
        set_in.insert(0);
        set_in.insert(1);
        set_in.insert(2);
        set_in.insert(1);
        set_in.insert(0);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(set_in);
        std::set<uint8_t> set_out { };
        sbs::deserialize_from_span(bytes, set_out);
        ASSERT(set_in == set_out);
    }

    test_section("std::multiset");
    {
        std::multiset<uint8_t> set_in { };
        set_in.insert(0);
        set_in.insert(1);
        set_in.insert(2);
        set_in.insert(1);
        set_in.insert(0);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(set_in);
        std::multiset<uint8_t> set_out;
        sbs::deserialize_from_span(bytes, set_out);
        ASSERT(set_in == set_out);
    }
}

inline void serialize_string()
{
    test_case("serialize <string>");

    test_section("std::string");
    {
        std::string str_in = "Hello World!";
        std::vector<std::byte> bytes = sbs::serialize_to_vector(str_in);
        std::string str_out { };
        sbs::deserialize_from_span(bytes, str_out);
        ASSERT(str_in == str_out);
    }

    test_section("std::u8string");
    {
        std::u8string str_in = u8"Hello 👋 World! 🌎";
        std::vector<std::byte> bytes = sbs::serialize_to_vector(str_in);
        std::u8string str_out { };
        sbs::deserialize_from_span(bytes, str_out);
        ASSERT(str_in == str_out);
    }

    test_section("std::u16string");
    {
        std::u16string str_in = u"Hello 👋 World! 🌎";
        std::vector<std::byte> bytes = sbs::serialize_to_vector(str_in);
        std::u16string str_out { };
        sbs::deserialize_from_span(bytes, str_out);
        ASSERT(str_in == str_out);
    }

    test_section("std::u32string");
    {
        std::u32string str_in = U"Hello 👋 World! 🌎";
        std::vector<std::byte> bytes = sbs::serialize_to_vector(str_in);
        std::u32string str_out { };
        sbs::deserialize_from_span(bytes, str_out);
        ASSERT(str_in == str_out);
    }
}

inline void serialize_unordered_map()
{
    test_case("serialize <unordered_map>");

    test_section("std::unordered_map");
    {
        std::unordered_map<std::string, uint16_t> map_in { };
        map_in.insert({ "one", 1 });
        map_in.insert({ "two", 4 });
        map_in.insert({ "three", 9 });
        map_in.insert({ "four", 16 });
        map_in.insert({ "two", 2 });
        std::vector<std::byte> bytes = sbs::serialize_to_vector(map_in);
        std::unordered_map<std::string, uint16_t> map_out { };
        sbs::deserialize_from_span(bytes, map_out);
        ASSERT(map_in == map_out);
    }

    test_section("std::unordered_multimap");
    {
        std::unordered_multimap<std::string, uint16_t> map_in { };
        map_in.insert({ "one", 1 });
        map_in.insert({ "three", 3 });
        map_in.insert({ "two", 4 });
        map_in.insert({ "one", 1 });
        map_in.insert({ "four", 4 });
        map_in.insert({ "three", 9 });
        map_in.insert({ "two", 2 });
        map_in.insert({ "four", 16 });
        std::vector<std::byte> bytes = sbs::serialize_to_vector(map_in);
        std::unordered_multimap<std::string, uint16_t> map_out { };
        sbs::deserialize_from_span(bytes, map_out);
        ASSERT(map_in == map_out);
    }
}

inline void serialize_unordered_set()
{
    test_case("serialize <unordered_set>");

    test_section("std::unordered_set");
    {
        std::unordered_set<std::string> set_in { };
        set_in.insert("zero");
        set_in.insert("one");
        set_in.insert("two");
        set_in.insert("one");
        set_in.insert("zero");
        std::vector<std::byte> bytes = sbs::serialize_to_vector(set_in);
        std::unordered_set<std::string> set_out { };
        sbs::deserialize_from_span(bytes, set_out);
        ASSERT(set_in == set_out);
    }

    test_section("std::unordered_multiset");
    {
        std::unordered_multiset<uint8_t> set_in { };
        set_in.insert(0);
        set_in.insert(1);
        set_in.insert(2);
        set_in.insert(1);
        set_in.insert(0);
        std::vector<std::byte> bytes = sbs::serialize_to_vector(set_in);
        std::unordered_multiset<uint8_t> set_out { };
        sbs::deserialize_from_span(bytes, set_out);
        ASSERT(set_in == set_out);
    }
}

inline void serialize_utility()
{
    test_case("serialize <utility>");

    test_section("std::pair");
    {
        std::pair<uint8_t, std::string> pair_in { 8, "eight" };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(pair_in);
        std::pair<uint8_t, std::string> pair_out { };
        sbs::deserialize_from_span(bytes, pair_out);
        ASSERT(pair_in == pair_out);
    }

    test_section("std::tuple");
    {
        std::tuple<uint8_t, std::string, double> tuple_in { 7, "seven", 7.0 };
        std::vector<std::byte> bytes = sbs::serialize_to_vector(tuple_in);
        std::tuple<uint8_t, std::string, double> tuple_out { };
        sbs::deserialize_from_span(bytes, tuple_out);
        ASSERT(tuple_in == tuple_out);
    }
}