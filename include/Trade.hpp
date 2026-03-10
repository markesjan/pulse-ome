#pragma once

#include <cstdint>

namespace engine {

struct Trade {
    std::string symbol;
    double timestamp;
    uint64_t tradeId;
    uint64_t buyOrderId;
    uint64_t sellOrderId;
    uint64_t price;
    uint32_t quantity;
};

} // namespace engine
