#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <unordered_set>

namespace sbs {

template <
    class Key,
    class KeySerializer = DefaultSerializer<Key>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct UnorderedSetSerializer {
    void operator()(Archive& ar, std::unordered_set<Key, Hash, KeyEqual, Allocator>& unordered_set) const
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
    class KeySerializer = DefaultSerializer<Key>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct UnorderedMultisetSerializer {
    void operator()(Archive& ar, std::unordered_multiset<Key, Hash, KeyEqual, Allocator>& unordered_multiset) const
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
    class KeySerializer = DefaultSerializer<Key>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key>)
void serialize(Archive& ar, std::unordered_set<Key, Hash, KeyEqual, Allocator>& unordered_set)
{
    UnorderedSetSerializer<Key, KeySerializer, Hash, KeyEqual, Allocator>()(ar, unordered_set);
}

template <
    class Key,
    class KeySerializer = DefaultSerializer<Key>,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key>)
void serialize(Archive& ar, std::unordered_multiset<Key, Hash, KeyEqual, Allocator>& unordered_multiset)
{
    UnorderedMultisetSerializer<Key, KeySerializer, Hash, KeyEqual, Allocator>()(ar, unordered_multiset);
}

}