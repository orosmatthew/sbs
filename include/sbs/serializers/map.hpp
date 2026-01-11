#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <map>

namespace sbs {

template <
    class Key,
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct MapSerializer {
    void operator()(Archive& ar, std::map<Key, Value, Compare, Allocator>& map) const
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
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(
        sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value> && std::copyable<Key>
        && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>)
struct MultimapSerializer {
    void operator()(Archive& ar, std::multimap<Key, Value, Compare, Allocator>& multimap) const
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
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value>)
void serialize(Archive& ar, std::map<Key, Value, Compare, Allocator>& map)
{
    MapSerializer<Key, Value, KeySerializer, ValueSerializer, Compare, Allocator>()(ar, map);
}

template <
    class Key,
    class Value,
    class KeySerializer = DefaultSerializer<Key>,
    class ValueSerializer = DefaultSerializer<Value>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, Value>>>
    requires(sbs::Serializer<KeySerializer, Key> && sbs::Serializer<ValueSerializer, Value>)
void serialize(Archive& ar, std::multimap<Key, Value, Compare, Allocator>& multimap)
{
    MultimapSerializer<Key, Value, KeySerializer, ValueSerializer, Compare, Allocator>()(ar, multimap);
}

}