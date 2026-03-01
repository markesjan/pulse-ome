#pragma once

#include <string>
#include <cstdint>

namespace engine {

struct Order {
    double timestamp;
    uint64_t orderId;
    uint64_t price;
    uint32_t quantity;
    uint8_t eventType;
    int8_t side;
    std::string symbol;
};

} // namespace engine
