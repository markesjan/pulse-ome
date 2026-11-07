/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  PriceBitset.hpp
 * @brief Definition of the PriceBitset class for efficient price level tracking.
 */

#pragma once

#include <vector>

#include "Types.hpp"

namespace engine {

class PriceBitset {
public:
    explicit PriceBitset(Price maxPrice);

    void setPrice(Price price);
    void clearPrice(Price price);
    Price findNextAsk(Price startPrice) const;
    Price findNextBid(Price startPrice) const;

private:
    std::vector<BitBlock> l0; // 1b = 1 price
    std::vector<BitBlock> l1; // 1b = 64 prices
    std::vector<BitBlock> l2; // 1b = 4096 prices
    std::vector<BitBlock> l3; // 1b = 262 144 prices
};

} // namespace engine
