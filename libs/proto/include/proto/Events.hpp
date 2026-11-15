/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Events.hpp
 * @brief Definitions of outbound (ome -> gui) event structures.
 */

#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>

#include <proto/Types.hpp>

namespace pulse::proto {

struct TradeEvent {
    Timestamp timestamp;
    TradeId tradeId;
    OrderId makerOrderId;
    OrderId takerOrderId;
    Price price;
    Quantity quantity;
    Side takerSide;
};

static_assert(std::is_trivially_copyable_v<TradeEvent>);
static_assert(std::is_standard_layout_v<TradeEvent>);

struct BookDepthEvent {
    Timestamp timestamp;
    Price price;
    Quantity totalQty;
    Side side;
    LevelIndex levelIdx;
};

static_assert(std::is_trivially_copyable_v<BookDepthEvent>);
static_assert(std::is_standard_layout_v<BookDepthEvent>);

struct LatencyTelemetryEvent {
    Timestamp timestamp;
    OrderId orderId;
    CpuCycles engineCycles;
};

static_assert(std::is_trivially_copyable_v<LatencyTelemetryEvent>);
static_assert(std::is_standard_layout_v<LatencyTelemetryEvent>);

struct EngineStatusEvent {
    Timestamp timestamp;
    SequenceNumber sequenceNumber;
    std::uint32_t ringOccupancy;
    EngineState engineState;
};

static_assert(std::is_trivially_copyable_v<EngineStatusEvent>);
static_assert(std::is_standard_layout_v<EngineStatusEvent>);

using EngineEvent =
    std::variant<TradeEvent, BookDepthEvent, LatencyTelemetryEvent, EngineStatusEvent>;

static_assert(sizeof(EngineEvent) <= EVENT_SIZE);
static_assert(std::is_trivially_copyable_v<EngineEvent>);

} // namespace pulse::proto
