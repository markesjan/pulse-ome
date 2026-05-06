#pragma once

#include <vector>
#include <cstdint>

#include "Order.hpp"

namespace engine {

class OrderGenerator {
public:
    static std::vector<OrderMessage> generate(uint32_t seed,
                                              std::size_t ordersCount,
                                              uint8_t cancelPercent,
                                              Price basePrice,
                                              double meanOffset,
                                              double stdDev);

};

} // namespace engine
