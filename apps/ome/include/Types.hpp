/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Types.hpp
 * @brief Common type definitions and enums for the matching engine.
 */

#pragma once

#include <cstdint>
#include <limits>

#include <proto/Types.hpp>

namespace engine {

using Timestamp = pulse::proto::Timestamp;
using OrderId = pulse::proto::OrderId;
using TradeId = pulse::proto::TradeId;
using Price = pulse::proto::Price;
using Quantity = pulse::proto::Quantity;
using EventType = pulse::proto::OrderEventType;
using Side = pulse::proto::Side;

using OrderIndex = uint32_t;
using BitBlock = uint64_t;

constexpr OrderIndex INVALID_INDEX = std::numeric_limits<OrderIndex>::max();
constexpr Price INVALID_PRICE = std::numeric_limits<Price>::max();

} // namespace engine
