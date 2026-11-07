/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  OrderBookLevel.hpp
 * @brief Definition of the OrderBookLevel structure for order book snapshots.
 */

#pragma once

#include "Types.hpp"

namespace engine {

struct OrderBookLevel {
    Price askPrice{0};
    Quantity askQty{0};
    Price bidPrice{0};
    Quantity bidQty{0};
};

} // namespace engine
