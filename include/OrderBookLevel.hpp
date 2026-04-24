#pragma once

#include "Types.hpp"

namespace engine {

struct OrderBookLevel {
    Price askPrice{0};
    Quantity askQty{0};
    Price bidPrice{0};
    Quantity bidQty{0};
};

} // namespace engine
