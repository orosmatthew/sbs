#include <sbs/sbs.hpp>

#include "test.hpp"

#include <array>
#include <chrono>
#include <complex>
#include <cstdint>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <optional>
#include <ratio>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>

enum class MyEnum : uint16_t { first, second, third };

template <
    class Element,
    class ElementSerializer = sbs::DefaultSerializer<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serializer<ElementSerializer, Element> && std::is_default_constructible_v<Element>)
struct VectorSerializer {
    void operator()(std::vector<Element, Allocator>& vector, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = vector.size();
            ar.archive_copy(size);
            for (Element& item : vector) {
                ar.archive<ElementSerializer>(item);
            }
        } else {
            vector.clear();
            uint64_t size = 0;
            ar.archive(size);
            vector.resize(size);
            for (Element& item : vector) {
                ar.archive<ElementSerializer>(item);
            }
        }
    }
};

template <
    class Char,
    class CharSerializer = sbs::DefaultSerializer<Char>,
    class Traits = std::char_traits<Char>,
    class Allocator = std::allocator<Char>>
    requires(sbs::Serializer<CharSerializer, Char> && std::is_default_constructible_v<Char>)
struct BasicStringSerializer {
    void operator()(std::basic_string<Char, Traits, Allocator>& string, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = string.size();
            ar.archive_copy(size);
            for (auto& element : string) {
                ar.archive<CharSerializer>(element);
            }
        } else {
            string.clear();
            uint64_t size = 0;
            ar.archive(size);
            string.resize(size);
            for (auto& element : string) {
                ar.archive<CharSerializer>(element);
            }
        }
    }
};

using StringSerializer = BasicStringSerializer<char>;

template <class Element, std::size_t Size, class ElementSerializer = sbs::DefaultSerializer<Element>>
    requires(sbs::Serializer<ElementSerializer, Element>)
struct ArraySerializer {
    void operator()(std::array<Element, Size>& array, sbs::Archive& ar) const
    {
        for (Element& element : array) {
            ar.archive<ElementSerializer>(element);
        }
    }
};

template <
    class Element,
    class ElementSerializer = sbs::DefaultSerializer<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serializer<ElementSerializer, Element> && std::is_default_constructible_v<Element>)
struct DequeSerializer {
    void operator()(std::deque<Element, Allocator>& deque, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = deque.size();
            ar.archive_copy(size);
            for (Element& element : deque) {
                ar.archive<ElementSerializer>(element);
            }
        } else {
            deque.clear();
            uint64_t size = 0;
            ar.archive(size);
            deque.resize(size);
            for (Element& element : deque) {
                ar.archive<ElementSerializer>(element);
            }
        }
    }
};

template <
    class Element,
    class ElementSerializer = sbs::DefaultSerializer<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serializer<ElementSerializer, Element> && std::is_default_constructible_v<Element>)
struct ForwardListSerializer {
    void operator()(std::forward_list<Element, Allocator>& forward_list, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = std::distance(forward_list.begin(), forward_list.end());
            ar.archive_copy(size);
            for (Element& element : forward_list) {
                ar.archive<ElementSerializer>(element);
            }
        } else {
            forward_list.clear();
            uint64_t size = 0;
            ar.archive(size);
            forward_list.resize(size);
            for (Element& element : forward_list) {
                ar.archive<ElementSerializer>(element);
            }
        }
    }
};

template <
    class Element,
    class ElementSerializer = sbs::DefaultSerializer<Element>,
    class Allocator = std::allocator<Element>>
    requires(sbs::Serializer<ElementSerializer, Element> && std::is_default_constructible_v<Element>)
struct ListSerializer {
    void operator()(std::list<Element, Allocator>& list, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = list.size();
            ar.archive_copy(size);
            for (Element& element : list) {
                ar.archive<ElementSerializer>(element);
            }
        } else {
            list.clear();
            uint64_t size = 0;
            ar.archive(size);
            list.resize(size);
            for (Element& element : list) {
                ar.archive<ElementSerializer>(element);
            }
        }
    }
};

template <
    class Key,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct SetSerializer {
    void operator()(std::set<Key, Compare, Allocator>& set, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = set.size();
            ar.archive_copy(size);
            for (const Key& key : set) {
                ar.archive_copy<KeySerializer>(key);
            }
        } else {
            set.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                set.insert(std::move(key));
            }
        }
    }
};

template <
    class Key,
    class Value,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class ValueSerializer = sbs::DefaultSerializer<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct MapSerializer {
    void operator()(std::map<Key, Value, Compare, Allocator>& map, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = map.size();
            ar.archive_copy(size);
            for (auto& [key, value] : map) {
                ar.archive_copy<KeySerializer>(key);
                ar.archive<ValueSerializer>(value);
            }
        } else {
            map.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                auto value = Value();
                ar.archive<ValueSerializer>(value);
                map.insert({ std::move(key), std::move(value) });
            }
        }
    }
};

template <
    class Key,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct MultisetSerializer {
    void operator()(std::multiset<Key, Compare, Allocator>& multiset, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = multiset.size();
            ar.archive_copy(size);
            for (const Key& key : multiset) {
                ar.archive_copy<KeySerializer>(key);
            }
        } else {
            multiset.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                multiset.insert(std::move(key));
            }
        }
    }
};

template <
    class Key,
    class Value,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class ValueSerializer = sbs::DefaultSerializer<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct MultimapSerializer {
    void operator()(std::multimap<Key, Value, Compare, Allocator>& multimap, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = multimap.size();
            ar.archive_copy(size);
            for (auto& [key, value] : multimap) {
                ar.archive_copy<KeySerializer>(key);
                ar.archive<ValueSerializer>(value);
            }
        } else {
            multimap.clear();
            uint64_t size = 0;
            ar.archive(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                auto value = Value();
                ar.archive<ValueSerializer>(value);
                multimap.insert({ std::move(key), std::move(value) });
            }
        }
    }
};

template <
    class Key,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct UnorderedSetSerializer {
    void operator()(std::unordered_set<Key, Hash, KeyEqual, Allocator>& unordered_set, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = unordered_set.size();
            ar.archive_copy(size);
            for (const Key& key : unordered_set) {
                ar.archive_copy<KeySerializer>(key);
            }
        } else {
            unordered_set.clear();
            uint64_t size = 0;
            ar.archive(size);
            unordered_set.reserve(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                unordered_set.insert(std::move(key));
            }
        }
    }
};

template <
    class Key,
    class Value,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class ValueSerializer = sbs::DefaultSerializer<Value>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct UnorderedMapSerializer {
    void operator()(std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map, sbs::Archive& ar)
    {
        if (ar.serializing()) {
            const uint64_t size = unordered_map.size();
            ar.archive_copy(size);
            for (auto& [key, value] : unordered_map) {
                ar.archive_copy<KeySerializer>(key);
                ar.archive<ValueSerializer>(value);
            }
        } else {
            unordered_map.clear();
            uint64_t size = 0;
            ar.archive(size);
            unordered_map.reserve(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                auto value = Value();
                ar.archive<ValueSerializer>(value);
                unordered_map.insert({ std::move(key), std::move(value) });
            }
        }
    }
};

template <
    class Key,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct UnorderedMultisetSerializer {
    void operator()(std::unordered_multiset<Key, Hash, KeyEqual, Allocator>& unordered_multiset, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = unordered_multiset.size();
            ar.archive_copy(size);
            for (const Key& key : unordered_multiset) {
                ar.archive_copy<KeySerializer>(key);
            }
        } else {
            unordered_multiset.clear();
            uint64_t size = 0;
            ar.archive(size);
            unordered_multiset.reserve(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                unordered_multiset.insert(std::move(key));
            }
        }
    }
};

template <
    class Key,
    class Value,
    class KeySerializer = sbs::DefaultSerializer<Key>,
    class ValueSerializer = sbs::DefaultSerializer<Value>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct UnorderedMultimapSerializer {
    void operator()(
        std::unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>& unordered_multimap, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t size = unordered_multimap.size();
            ar.archive_copy(size);
            for (auto& [key, value] : unordered_multimap) {
                ar.archive_copy<KeySerializer>(key);
                ar.archive<ValueSerializer>(value);
            }
        } else {
            unordered_multimap.clear();
            uint64_t size = 0;
            ar.archive(size);
            unordered_multimap.reserve(size);
            for (uint64_t i = 0; i < size; ++i) {
                auto key = Key();
                ar.archive<KeySerializer>(key);
                auto value = Value();
                ar.archive<ValueSerializer>(value);
                unordered_multimap.insert({ std::move(key), std::move(value) });
            }
        }
    }
};

template <class Value, class ValueSerializer = sbs::DefaultSerializer<Value>>
    requires(sbs::Serializer<ValueSerializer, Value> && std::is_default_constructible_v<Value>)
struct OptionalSerializer {
    void operator()(std::optional<Value>& optional, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const bool has_value = optional.has_value();
            ar.archive_copy(has_value);
            if (has_value) {
                ar.archive<ValueSerializer>(optional.value());
            }
        } else {
            optional.reset();
            bool has_value = false;
            ar.archive(has_value);
            if (has_value) {
                auto value = Value();
                ar.archive<ValueSerializer>(value);
                optional = value;
            }
        }
    }
};

template <class Variant, uint64_t Index = 0>
void construct_variant_at_index(Variant& variant, const uint64_t index)
{
    if constexpr (Index >= std::variant_size_v<Variant>) {
    } else if (Index == index) {
        using Type = std::variant_alternative_t<Index, Variant>;
        static_assert(std::is_default_constructible_v<Type>);
        auto value = Type();
        variant = value;
    } else {
        return construct_variant_at_index<Variant, Index + 1>(variant, index);
    }
}

template <class... Types>
    requires((sbs::DefaultSerializable<Types> && std::is_default_constructible_v<Types>) && ...)
struct VariantDefaultSerializer {
    void operator()(std::variant<Types...>& variant, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const uint64_t index = variant.index();
            ar.archive_copy(index);
            std::visit([&]<class T>(T& value) { ar.archive(value); }, variant);
        } else {
            uint64_t index = 0;
            ar.archive(index);
            construct_variant_at_index<std::variant<Types...>>(variant, index);
            std::visit([&]<class T>(T& value) { ar.archive(value); }, variant);
        }
    }
};

template <
    class First,
    class Second,
    class FirstSerializer = sbs::DefaultSerializer<First>,
    class SecondSerializer = sbs::DefaultSerializer<Second>>
    requires(sbs::Serializer<FirstSerializer, First> && sbs::Serializer<SecondSerializer, Second>)
struct PairSerializer {
    void operator()(std::pair<First, Second>& pair, sbs::Archive& ar) const
    {
        ar.archive<FirstSerializer>(pair.first);
        ar.archive<SecondSerializer>(pair.second);
    }
};

template <class Tick, class TickSerializer = sbs::DefaultSerializer<Tick>, class Period = std::ratio<1>>
    requires(sbs::Serializer<TickSerializer, Tick> && std::copyable<Tick> && std::is_default_constructible_v<Tick>)
struct ChronoDurationSerializer {
    void operator()(std::chrono::duration<Tick, Period>& duration, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const Tick ticks = duration.count();
            ar.archive_copy<TickSerializer>(ticks);
        } else {
            auto ticks = Tick();
            ar.archive<TickSerializer>(ticks);
            duration = std::chrono::duration<Tick, Period> { ticks };
        }
    }
};

template <class Clock, class Duration = Clock::duration, class DurationSerializer = sbs::DefaultSerializer<Duration>>
    requires(
        sbs::Serializer<DurationSerializer, Duration> && std::copyable<Duration>
        && std::is_default_constructible_v<Duration>)
struct ChronoTimePointSerializer {
    void operator()(std::chrono::time_point<Clock, Duration>& time_point, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            const Duration ticks = time_point.time_since_epoch();
            ar.archive_copy<DurationSerializer>(ticks);
        } else {
            auto duration = Duration();
            ar.archive<DurationSerializer>(duration);
            time_point = std::chrono::time_point<Clock, Duration> { duration };
        }
    }
};

template <class Tuple, std::size_t Index = 0>
void serialize_tuple(Tuple& tuple, sbs::Archive& ar)
{
    if constexpr (Index < std::tuple_size_v<Tuple>) {
        ar.archive(std::get<Index>(tuple));
        serialize_tuple<Tuple, Index + 1>(tuple, ar);
    }
}

template <class... Types>
    requires(sbs::DefaultSerializable<Types> && ...)
struct TupleDefaultSerializer {
    void operator()(std::tuple<Types...>& tuple, sbs::Archive& ar) const
    {
        serialize_tuple(tuple, ar);
    }
};

template <class Type, class TypeSerializer = sbs::DefaultSerializer<Type>>
    requires(sbs::Serializer<TypeSerializer, Type> && std::copyable<Type> && std::is_default_constructible_v<Type>)
struct ComplexSerializer {
    void operator()(std::complex<Type>& complex, sbs::Archive& ar) const
    {
        if (ar.serializing()) {
            ar.archive_copy<TypeSerializer>(complex.real());
            ar.archive_copy<TypeSerializer>(complex.imag());
        } else {
            auto real = Type();
            ar.archive<TypeSerializer>(real);
            complex.real(real);
            auto imag = Type();
            ar.archive<TypeSerializer>(imag);
            complex.imag(imag);
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

    void serialize(sbs::Archive& ar)
    {
        ar.archive(a);
        ar.archive(b);
        ar.archive(c);
        ar.archive(d);
        ar.archive<VectorSerializer<uint16_t>>(numbers);
        ar.archive<VectorSerializer<std::vector<uint8_t>, VectorSerializer<uint8_t>>>(multi_nums);
        ar.archive<StringSerializer>(str);
        ar.archive<ArraySerializer<uint8_t, 4>>(nums_array);
        ar.archive(my_enum);
        ar.archive<DequeSerializer<uint8_t>>(deque);
        ar.archive<ForwardListSerializer<uint8_t>>(forward_list);
        ar.archive<ListSerializer<uint8_t>>(list);
        ar.archive<SetSerializer<uint8_t>>(set);
        ar.archive<MapSerializer<std::string, uint16_t, StringSerializer>>(map);
        ar.archive<MultisetSerializer<uint8_t>>(multiset);
        ar.archive<MultimapSerializer<std::string, uint16_t, StringSerializer>>(multimap);
        ar.archive<UnorderedSetSerializer<std::string, StringSerializer>>(unordered_set);
        ar.archive<UnorderedMapSerializer<std::string, uint16_t, StringSerializer>>(unordered_map);
        ar.archive<UnorderedMultisetSerializer<uint8_t>>(unordered_multiset);
        ar.archive<UnorderedMultimapSerializer<std::string, uint16_t, StringSerializer>>(unordered_multimap);
        ar.archive<OptionalSerializer<uint8_t>>(optional);
        ar.archive<VariantDefaultSerializer<uint8_t, float>>(variant1);
        ar.archive<VariantDefaultSerializer<uint8_t, float>>(variant2);
        ar.archive<PairSerializer<uint8_t, std::string, sbs::DefaultSerializer<uint8_t>, StringSerializer>>(pair);
        ar.archive<TupleDefaultSerializer<int, double, float>>(tuple);
        ar.archive<ChronoDurationSerializer<double>>(duration);
        ar.archive<ChronoTimePointSerializer<
            std::chrono::steady_clock,
            std::chrono::steady_clock::duration,
            ChronoDurationSerializer<
                std::chrono::steady_clock::duration::rep,
                sbs::DefaultSerializer<std::chrono::steady_clock::rep>,
                std::chrono::steady_clock::duration::period>>>(time_point);
        ar.archive<ComplexSerializer<float>>(complex);
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

    uint64_t thing = 1024;
    std::vector<std::byte> thing_bytes = sbs::serialize_to_vector(thing);

    std::vector<std::byte> bytes = sbs::serialize_to_vector(s);

    SimpleStruct out { };
    sbs::deserialize_from_span(out, bytes);

    uint64_t thing_out;
    sbs::deserialize_from_span(thing_out, thing_bytes);

    END_TESTS;
}
