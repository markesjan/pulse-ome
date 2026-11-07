/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Trade.hpp
 * @brief Definition of the Trade structure.
 */

#pragma once

#include "Types.hpp"

namespace engine {

struct Trade {
    Timestamp timestamp;
    TradeId tradeId;
    OrderId buyOrderId;
    OrderId sellOrderId;
    Price price;
    Quantity quantity;
};

} // namespace engine
