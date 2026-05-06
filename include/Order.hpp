#pragma once

#include "Types.hpp"

namespace engine {

struct OrderMessage {
    Timestamp timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    EventType eventType;
    Side side;
};

struct alignas(32) Order {
    Timestamp timestamp;
    OrderId orderId;
    Price price;
    Quantity quantity;
    OrderIndex nextOrderIdx;
};

static_assert(sizeof(Order) == 32, "Order struct not aligned with cache line!");

} // namespace engine
