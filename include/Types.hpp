#pragma once

#include <cstdint>

namespace engine {

using OrderId = uint64_t;
using TradeId = uint64_t;
using Price = uint64_t;
using Quantity = uint32_t;

enum class EventType : uint8_t {
    New = 1,
    PartialCancel = 2,
    FullCancel = 3,
    Execution = 4,
    HiddenExecution = 5,
};

enum class Side : int8_t {
    Sell = -1,
    Buy = 1
};

} // namespace engine
