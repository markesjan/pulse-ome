/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Order.hpp
 * @brief Definition of internal Order structure.
 */

#pragma once

#include "Types.hpp"
#include "proto/Messages.hpp"

namespace engine {

using OrderMessage = pulse::proto::OrderMessage;

struct alignas(32) Order {
    Timestamp timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    OrderIndex nextOrderIdx;
};

static_assert(sizeof(Order) == 32, "Order struct not aligned with cache line!");

} // namespace engine
