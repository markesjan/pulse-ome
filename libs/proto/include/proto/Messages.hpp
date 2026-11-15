/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Messages.hpp
 * @brief Definition of inbound (feed -> ome) message structure.
 */

#pragma once

#include <type_traits>

#include <proto/Types.hpp>

namespace pulse::proto {

struct alignas(MESSAGE_SIZE) OrderMessage {
    Timestamp timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    OrderAction action;
    Side side;
};

static_assert(sizeof(OrderMessage) == MESSAGE_SIZE);
static_assert(std::is_trivially_copyable_v<OrderMessage>);
static_assert(std::is_standard_layout_v<OrderMessage>);

} // namespace pulse::proto
