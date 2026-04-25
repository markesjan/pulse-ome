#pragma once

#include <string>
#include <cstdint>

namespace engine {

struct Config {
    std::string orderFile;
    std::string tradeFile;
    std::size_t generateCnt = 1'000'000;
    uint32_t seed = 42;
    bool measureLatency = false;
};

} // namespace engine
