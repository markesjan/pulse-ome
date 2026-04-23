#pragma once

#include "Types.hpp"

namespace engine {

struct Order {
    double timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    EventType eventType;
    Side side;
};

} // namespace engine
