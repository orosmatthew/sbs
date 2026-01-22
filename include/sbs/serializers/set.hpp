#ifndef SBS_SERIALIZERS_SET_HPP
#define SBS_SERIALIZERS_SET_HPP

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
    void operator()(Archive& ar, std::set<Key, Compare, Allocator>& set) const
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
    void operator()(Archive& ar, std::multiset<Key, Compare, Allocator>& multiset) const
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
void serialize(Archive& ar, std::set<Key, Compare, Allocator>& set)
{
    SetSerializer<Key, KeySerializer, Compare, Allocator>()(ar, set);
}

template <
    class Key,
    class KeySerializer = DefaultSerializer<Key>,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<Key>>
    requires(sbs::Serializer<KeySerializer, Key> && std::copyable<Key> && std::is_default_constructible_v<Key>)
void serialize(Archive& ar, std::multiset<Key, Compare, Allocator>& multiset)
{
    MultisetSerializer<Key, KeySerializer, Compare, Allocator>()(ar, multiset);
}

}

#endif // SBS_SERIALIZERS_SET_HPP