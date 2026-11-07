/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  PriceBitset.cpp
 * @brief Implementation of the PriceBitset class.
 */

#include "PriceBitset.hpp"

namespace engine {

PriceBitset::PriceBitset(Price maxPrice) {
    l0.assign((maxPrice >> 6) + 1, 0);
    l1.assign((maxPrice >> 12) + 1, 0);
    l2.assign((maxPrice >> 18) + 1, 0);
    l3.assign((maxPrice >> 24) + 1, 0);
}

void PriceBitset::setPrice(Price price) {
    l0[price >> 6] |= (1ULL << (price & 63));
    l1[price >> 12] |= (1ULL << ((price >> 6) & 63));
    l2[price >> 18] |= (1ULL << ((price >> 12) & 63));
    l3[price >> 24] |= (1ULL << ((price >> 18) & 63));
}

void PriceBitset::clearPrice(Price price) {
    l0[price >> 6] &= ~(1ULL << (price & 63));
    if (l0[price >> 6] == 0) {
        l1[price >> 12] &= ~(1ULL << ((price >> 6) & 63));
        if (l1[price >> 12] == 0) {
            l2[price >> 18] &= ~(1ULL << ((price >> 12) & 63));
            if (l2[price >> 18] == 0) {
                l3[price >> 24] &= ~(1ULL << ((price >> 18) & 63));
            }
        }
    }
}

Price PriceBitset::findNextAsk(Price startPrice) const {
    uint64_t i0 = startPrice >> 6;
    if (i0 >= l0.size())
        return UINT64_MAX;

    uint64_t mask0 = ~((1ULL << (startPrice & 63)) - 1);
    BitBlock block0 = l0[i0] & mask0;
    if (block0)
        return (i0 << 6) + static_cast<Price>(__builtin_ctzll(block0));

    uint64_t startl1 = i0 + 1;
    uint64_t i1 = startl1 >> 6;
    if (i1 >= l1.size())
        return UINT64_MAX;

    uint64_t mask1 = ~((1ULL << (startl1 & 63)) - 1);
    BitBlock block1 = l1[i1] & mask1;
    if (block1) {
        uint64_t nextl0 = (i1 << 6) + static_cast<Price>(__builtin_ctzll(block1));
        return (nextl0 << 6) + static_cast<Price>(__builtin_ctzll(l0[nextl0]));
    }

    uint64_t startl2 = i1 + 1;
    uint64_t i2 = startl2 >> 6;
    if (i2 >= l2.size())
        return UINT64_MAX;

    uint64_t mask2 = ~((1ULL << (startl2 & 63)) - 1);
    BitBlock block2 = l2[i2] & mask2;
    if (block2) {
        uint64_t nextl1 = (i2 << 6) + static_cast<Price>(__builtin_ctzll(block2));
        uint64_t nextl0 = (nextl1 << 6) + static_cast<Price>(__builtin_ctzll(l1[nextl1]));
        return (nextl0 << 6) + static_cast<Price>(__builtin_ctzll(l0[nextl0]));
    }

    uint64_t startl3 = i2 + 1;
    uint64_t i3 = startl3 >> 6;
    if (i3 >= l3.size())
        return UINT64_MAX;

    uint64_t mask3 = ~((1ULL << (startl3 & 63)) - 1);
    BitBlock block3 = l3[i3] & mask3;
    if (block3) {
        uint64_t nextl2 = (i3 << 6) + static_cast<Price>(__builtin_ctzll(block3));
        uint64_t nextl1 = (nextl2 << 6) + static_cast<Price>(__builtin_ctzll(l2[nextl2]));
        uint64_t nextl0 = (nextl1 << 6) + static_cast<Price>(__builtin_ctzll(l1[nextl1]));
        return (nextl0 << 6) + static_cast<Price>(__builtin_ctzll(l0[nextl0]));
    }

    return UINT64_MAX;
}

Price PriceBitset::findNextBid(Price startPrice) const {
    uint64_t i0 = startPrice >> 6;
    if (i0 >= l0.size())
        i0 = l0.size() - 1;

    uint64_t bitPos0 = startPrice & 63;
    uint64_t mask0 = (bitPos0 == 63) ? ~0ULL : ((1ULL << (bitPos0 + 1)) - 1);
    BitBlock block0 = l0[i0] & mask0;
    if (block0)
        return (i0 << 6) + 63 - static_cast<Price>(__builtin_clzll(block0));

    if (i0 == 0)
        return UINT64_MAX;

    uint64_t startl1 = i0 - 1;
    uint64_t i1 = startl1 >> 6;
    uint64_t bitPos1 = startl1 & 63;
    uint64_t mask1 = (bitPos1 == 63) ? ~0ULL : ((1ULL << (bitPos1 + 1)) - 1);
    BitBlock block1 = l1[i1] & mask1;

    if (block1) {
        uint64_t nextl0 = (i1 << 6) + 63 - static_cast<Price>(__builtin_clzll(block1));
        return (nextl0 << 6) + 63 - static_cast<Price>(__builtin_clzll(l0[nextl0]));
    }

    if (i1 == 0)
        return UINT64_MAX;

    uint64_t startl2 = i1 - 1;
    uint64_t i2 = startl2 >> 6;
    uint64_t bitPos2 = startl2 & 63;
    uint64_t mask2 = (bitPos2 == 63) ? ~0ULL : ((1ULL << (bitPos2 + 1)) - 1);
    BitBlock block2 = l2[i2] & mask2;

    if (block2) {
        uint64_t nextl1 = (i2 << 6) + 63 - static_cast<Price>(__builtin_clzll(block2));
        uint64_t nextl0 = (nextl1 << 6) + 63 - static_cast<Price>(__builtin_clzll(l1[nextl1]));
        return (nextl0 << 6) + 63 - static_cast<Price>(__builtin_clzll(l0[nextl0]));
    }

    if (i2 == 0)
        return UINT64_MAX;

    uint64_t startl3 = i2 - 1;
    uint64_t i3 = startl3 >> 6;
    uint64_t bitPos3 = startl3 & 63;
    uint64_t mask3 = (bitPos3 == 63) ? ~0ULL : ((1ULL << (bitPos3 + 1)) - 1);
    BitBlock block3 = l3[i3] & mask3;

    if (block3) {
        uint64_t nextl2 = (i3 << 6) + 63 - static_cast<Price>(__builtin_clzll(block3));
        uint64_t nextl1 = (nextl2 << 6) + 63 - static_cast<Price>(__builtin_clzll(l2[nextl2]));
        uint64_t nextl0 = (nextl1 << 6) + 63 - static_cast<Price>(__builtin_clzll(l1[nextl1]));
        return (nextl0 << 6) + 63 - static_cast<Price>(__builtin_clzll(l0[nextl0]));
    }

    return UINT64_MAX;
}

} // namespace engine
