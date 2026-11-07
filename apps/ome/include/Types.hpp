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

namespace engine {

using Timestamp = uint64_t;
using OrderId = uint64_t;
using TradeId = uint64_t;
using Price = uint64_t;
using Quantity = uint32_t;
using OrderIndex = uint32_t;
using BitBlock = uint64_t;

constexpr OrderIndex INVALID_INDEX = std::numeric_limits<OrderIndex>::max();
constexpr Price INVALID_PRICE = std::numeric_limits<Price>::max();

enum class EventType : uint8_t {
    Invalid = 0,
    New = 1,
    PartialCancel = 2,
    FullCancel = 3,
    Execution = 4,
    HiddenExecution = 5,
};

enum class Side : int8_t { Sell = -1, Buy = 1 };

} // namespace engine
