#pragma once

#include <vector>
#include <cstdint>

#include "Order.hpp"

namespace engine {

class OrderGenerator {
public:
    static std::vector<Order> generate(uint32_t seed,
                                       size_t ordersCnt,
                                       size_t cancelPercentage = 20,
                                       Price basePrice = 10000);
};

} // namespace engine
