/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Messages.hpp
 * @brief Definitions of inbound message structures.
 */

#pragma once

#include "Types.hpp"

namespace pulse::proto {

struct OrderMessage {
    Timestamp timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    OrderEventType eventType;
    Side side;
};

static_assert(sizeof(OrderMessage) == 32, "OrderMessage not aligned with cache line!");

} // namespace pulse::proto
