#pragma once

#include <string>
#include <cstdint>

namespace engine {

struct Config {
    std::string inFile;
    std::string outFile;
    size_t generateCnt = 100000;
    uint32_t seed = 42;
};

} // namespace engine
