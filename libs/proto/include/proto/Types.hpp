#pragma once

#include <cstdint>

namespace pulse::proto {

using Timestamp = uint64_t;
using OrderId = uint64_t;
using TradeId = uint64_t;
using Price = uint64_t;
using Quantity = uint32_t;

enum class OrderEventType : uint8_t {
    Invalid = 0,
    New = 1,
    PartialCancel = 2,
    FullCancel = 3,
    Execution = 4,
    HiddenExecution = 5,
};

enum class Side : int8_t { Sell = -1, Buy = 1 };

} // namespace pulse::proto
