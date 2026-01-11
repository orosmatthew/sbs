#pragma once

#include <sbs/sbs.hpp>

#include <bitset>
#include <cstdint>

namespace sbs {

template <std::size_t size>
struct BitsetSerializer {
    void operator()(std::bitset<size>& bitset, Archive& ar) const
    {
        if (ar.serializing()) {
            uint8_t buffer = 0;
            for (size_t i = 0; i < bitset.size(); ++i) {
                if (bitset.test(i)) {
                    buffer |= 1 << (i % 8);
                }
                if (i % 8 == 7 || i == bitset.size() - 1) {
                    ar.archive<uint8_t>(buffer);
                    buffer = 0;
                }
            }
        } else {
            bitset.reset();
            uint8_t buffer = 0;
            for (size_t i = 0; i < bitset.size(); ++i) {
                if (i % 8 == 0) {
                    ar.archive<uint8_t>(buffer);
                }
                if (buffer & 1 << i % 8) {
                    bitset.set(i, true);
                }
            }
        }
    }
};

template <std::size_t size>
void serialize(std::bitset<size>& bitset, Archive& ar)
{
    BitsetSerializer<size>()(bitset, ar);
}

}