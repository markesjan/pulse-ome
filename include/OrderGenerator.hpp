#pragma once

#include <vector>
#include <cstdint>

#include "Order.hpp"

namespace engine {

class OrderGenerator {
public:
    static std::vector<Order> generate(uint32_t seed,
                                       size_t ordersCnt,
                                       uint64_t basePrice = 10000,
                                       size_t cancelPercentage = 20);
};

} // namespace engine
