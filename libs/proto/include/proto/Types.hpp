/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Types.hpp
 * @brief Definitions of common types and aliases.
 */

#pragma once

#include <cstdint>
#include <new>

namespace pulse::proto {

constexpr std::uint8_t CACHE_LINE_SIZE = std::hardware_constructive_interference_size;
constexpr std::uint8_t EVENT_SIZE = CACHE_LINE_SIZE;
constexpr std::uint8_t MESSAGE_SIZE = CACHE_LINE_SIZE / 2;
constexpr std::uint8_t COMMAND_SIZE = CACHE_LINE_SIZE / 4;

using Timestamp = std::uint64_t;
using OrderId = std::uint64_t;
using TradeId = std::uint64_t;
using Price = std::uint64_t;
using Quantity = std::uint32_t;
using CpuCycles = std::uint64_t;
using SequenceNumber = std::uint64_t;
using LevelIndex = std::uint32_t;
using RingBufferIndex = std::uint64_t;

enum class OrderAction : std::uint8_t {
    Invalid = 0,
    New = 1,
    PartialCancel = 2,
    FullCancel = 3,
    Execution = 4,
    HiddenExecution = 5,
};

enum class Side : std::int8_t {
    Sell = -1,
    Buy = 1,
};

enum class EngineState : std::uint8_t {
    Offline = 0,
    Starting = 1,
    Running = 2,
    Stopping = 3,
};

enum class FeedAction : std::uint8_t {
    Invalid = 0,
    Play = 1,
    Pause = 2,
    Step = 3,
    Stop = 4,
    SetSpeed = 5,
    ChangeStrategy = 6,
};

enum class FeedSource : std::uint8_t {
    Invalid = 0,
    Lobster = 1,
    Synthetic = 2,
    Binance = 3,
};

} // namespace pulse::proto
