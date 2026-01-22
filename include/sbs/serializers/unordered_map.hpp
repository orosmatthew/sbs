#ifndef SBS_SERIALIZERS_UNORDERED_MAP_HPP
#define SBS_SERIALIZERS_UNORDERED_MAP_HPP

#include <sbs/sbs.hpp>

#include <cstdint>
#include <unordered_map>

namespace sbs {

template <
    class Key,
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct UnorderedMapSerializer {
    void operator()(Archive& ar, std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map) const
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
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct UnorderedMultimapSerializer {
    void operator()(
        Archive& ar, std::unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>& unordered_multimap) const
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

template <
    class Key,
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value>)
void serialize(Archive& ar, std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map)
{
    UnorderedMapSerializer<Key, Value, KeySerializer, ValueSerializer, Hash, KeyEqual, Allocator>()(ar, unordered_map);
}

template <
    class Key,
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value>)
void serialize(Archive& ar, std::unordered_multimap<Key, Value, Hash, KeyEqual, Allocator>& unordered_multimap)
{
    UnorderedMultimapSerializer<Key, Value, KeySerializer, ValueSerializer, Hash, KeyEqual, Allocator>()(
        ar, unordered_multimap);
}

}

#endif // SBS_SERIALIZERS_UNORDERED_MAP_HPP