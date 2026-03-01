#pragma once

#include <cstdint>
#include <string>

namespace engine {

struct Trade {
    // double timestamp;
    // uint64_t tradeId;
    uint64_t buyOrderId;
    uint64_t sellOrderId;
    uint64_t price;
    uint32_t quantity;
    std::string symbol;
};

} // namespace engine
