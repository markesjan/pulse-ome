#pragma once

#include <vector>
#include <cstdint>

#include "Order.hpp"

namespace engine {

class OrderGenerator {
public:
    static std::vector<Order> generate(uint32_t seed,
                                       std::size_t ordersCnt,
                                       std::size_t cancelPercentage = 20,
                                       Price basePrice = 10000);
};

} // namespace engine
