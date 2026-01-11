#pragma once

#include <sbs/sbs.hpp>

#include <cstdint>
#include <set>

namespace sbs {

template <
    class Key,
    class KeySerializer = DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct SetSerializer {
    void operator()(std::set<Key, Compare, Allocator>& set, Archive& ar) const
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
    class KeySerializer = DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
struct MultisetSerializer {
    void operator()(std::multiset<Key, Compare, Allocator>& multiset, Archive& ar) const
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
    class KeySerializer = DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
void serialize(std::set<Key, Compare, Allocator>& set, Archive& ar)
{
    SetSerializer<Key, KeySerializer, Compare, Allocator>()(set, ar);
}

template <
    class Key,
    class KeySerializer = DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
void serialize(std::multiset<Key, Compare, Allocator>& multiset, Archive& ar)
{
    MultisetSerializer<Key, KeySerializer, Compare, Allocator>()(multiset, ar);
}

}