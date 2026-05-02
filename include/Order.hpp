#pragma once

#include "Types.hpp"

namespace engine {

struct Order {
    Timestamp timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    EventType eventType;
    Side side;

    OrderIndex nextOrderIdx = INVALID_INDEX;
};

} // namespace engine
