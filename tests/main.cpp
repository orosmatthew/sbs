#include <sbs/sbs.hpp>

#include "test.hpp"

#include <array>
#include <cstdint>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>

enum class MyEnum : uint16_t { first, second, third };

template <
    class Element,
    class ElementSerialize = sbs::DefaultSerialize<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serialize<ElementSerialize, Element> && std::is_default_constructible_v<Element>)
struct VectorSerialize {
    void operator()(std::vector<Element, Allocator>& vector, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = vector.size();
            ar.archive_copy(size);
            for (Element& item : vector) {
                ar.archive<ElementSerialize>(item);
            }
        } else {
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
    requires(sbs::Serialize<CharSerialize, Char> && std::is_default_constructible_v<Char>)
struct BasicStringSerialize {
    void operator()(std::basic_string<Char, Traits, Allocator>& string, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = string.size();
            ar.archive_copy(size);
            for (auto& element : string) {
                ar.archive<CharSerialize>(element);
            }
        } else {
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
    void operator()(std::array<Element, Size>& array, sbs::Archive& ar) const
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
    requires(sbs::Serialize<ElementSerialize, Element> && std::is_default_constructible_v<Element>)
struct DequeSerialize {
    void operator()(std::deque<Element, Allocator>& deque, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = deque.size();
            ar.archive_copy(size);
            for (Element& element : deque) {
                ar.archive<ElementSerialize>(element);
            }
        } else {
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

template <
    class Element,
    class ElementSerialize = sbs::DefaultSerialize<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serialize<ElementSerialize, Element> && std::is_default_constructible_v<Element>)
struct ForwardListSerialize {
    void operator()(std::forward_list<Element, Allocator>& forward_list, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = std::distance(forward_list.begin(), forward_list.end());
            ar.archive_copy(size);
            for (Element& element : forward_list) {
                ar.archive<ElementSerialize>(element);
            }
        } else {
            forward_list.clear();
            uint64_t size = 0;
            ar.archive(size);
            forward_list.resize(size);
            for (Element& element : forward_list) {
                ar.archive<ElementSerialize>(element);
            }
        }
    }
};

template <
    class Element,
    class ElementSerialize = sbs::DefaultSerialize<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serialize<ElementSerialize, Element> && std::is_default_constructible_v<Element>)
struct ListSerialize {
    void operator()(std::list<Element, Allocator>& list, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = list.size();
            ar.archive_copy(size);
            for (Element& element : list) {
                ar.archive<ElementSerialize>(element);
            }
        } else {
            list.clear();
            uint64_t size = 0;
            ar.archive(size);
            list.resize(size);
            for (Element& element : list) {
                ar.archive<ElementSerialize>(element);
            }
        }
    }
};

template <
    class Key,
    class KeySerialize = sbs::DefaultSerialize<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serialize<KeySerialize, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct SetSerialize {
    void operator()(std::set<Key, Compare, Allocator>& set, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = set.size();
            ar.archive_copy(size);
            for (const Key& key : set) {
                ar.archive_copy<KeySerialize>(key);
            }
        } else {
            set.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerialize>(key);
                set.insert(std::move(key));
            }
        }
    }
};

template <
    class Key,
    class Value,
    class KeySerialize = sbs::DefaultSerialize<Key>,
    class ValueSerialize = sbs::DefaultSerialize<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serialize<KeySerialize, Key> && sbs::Serialize<ValueSerialize, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct MapSerialize {
    void operator()(std::map<Key, Value, Compare, Allocator>& map, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = map.size();
            ar.archive_copy(size);
            for (auto& [key, value] : map) {
                ar.archive_copy<KeySerialize>(key);
                ar.archive<ValueSerialize>(value);
            }
        } else {
            map.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerialize>(key);
                auto value = Value();
                ar.archive<ValueSerialize>(value);
                map[std::move(key)] = std::move(value);
            }
        }
    }
};

template <
    class Key,
    class KeySerialize = sbs::DefaultSerialize<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serialize<KeySerialize, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct MultisetSerialize {
    void operator()(std::multiset<Key, Compare, Allocator>& multiset, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = multiset.size();
            ar.archive_copy(size);
            for (const Key& key : multiset) {
                ar.archive_copy<KeySerialize>(key);
            }
        } else {
            multiset.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerialize>(key);
                multiset.insert(std::move(key));
            }
        }
    }
};

template <
    class Key,
    class Value,
    class KeySerialize = sbs::DefaultSerialize<Key>,
    class ValueSerialize = sbs::DefaultSerialize<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serialize<KeySerialize, Key> && sbs::Serialize<ValueSerialize, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct MultimapSerialize {
    void operator()(std::multimap<Key, Value, Compare, Allocator>& multimap, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = multimap.size();
            ar.archive_copy(size);
            for (auto& [key, value] : multimap) {
                ar.archive_copy<KeySerialize>(key);
                ar.archive<ValueSerialize>(value);
            }
        } else {
            multimap.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerialize>(key);
                auto value = Value();
                ar.archive<ValueSerialize>(value);
                multimap.insert({ std::move(key), std::move(value) });
            }
        }
    }
};

struct OtherStruct {
    uint64_t thing;
};

void serialize(OtherStruct& o, sbs::Archive& ar)
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

    void serialize(sbs::Archive& ar)
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
        ar.archive<ForwardListSerialize<uint8_t>>(forward_list);
        ar.archive<ListSerialize<uint8_t>>(list);
        ar.archive<SetSerialize<uint8_t>>(set);
        ar.archive<MapSerialize<std::string, uint16_t, StringSerialize>>(map);
        ar.archive<MultisetSerialize<uint8_t>>(multiset);
        ar.archive<MultimapSerialize<std::string, uint16_t, StringSerialize>>(multimap);
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
        .multimap { }
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

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
