/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  OrderGenerator.hpp
 * @brief Declaration of the OrderGenerator class for synthetic order generation.
 */

#pragma once

#include <cstdint>
#include <vector>

#include "Order.hpp"

namespace engine {

class OrderGenerator {
public:
    static std::vector<OrderMessage> generate(uint32_t seed,
                                              std::size_t ordersCount,
                                              uint8_t cancelPercent,
                                              Price basePrice,
                                              double meanOffset,
                                              double stdDev);
};

} // namespace engine
