#pragma once

#include "Types.hpp"

namespace engine {

struct Trade {
    double timestamp;
    TradeId tradeId;
    OrderId buyOrderId;
    OrderId sellOrderId;
    Price price;
    Quantity quantity;
};

} // namespace engine
