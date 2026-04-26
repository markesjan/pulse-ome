#pragma once

#include <string>
#include <cstdint>

#include "Types.hpp"

namespace engine {

struct Config {
    std::string orderFile;
    std::string tradeFile;
    bool measureLatency = false;
    uint32_t genSeed = 42;
    std::size_t genCount = 100'000;
    uint8_t genCancelPercent = 20;
    Price genBasePrice = 10'000;
    double genMeanOffset = 14.0;
    double genStdDev = 8.0;
};

} // namespace engine
