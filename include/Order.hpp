#pragma once

#include <cstdint>
#include <string>

namespace engine {

struct Order {
    std::string symbol;
    double timestamp;
    uint64_t orderId;
    uint64_t price;
    uint32_t quantity;
    uint8_t eventType;
    int8_t side;
};

} // namespace engine
