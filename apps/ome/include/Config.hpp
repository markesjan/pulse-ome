/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Config.hpp
 * @brief Definition of the Config structure for program settings.
 */

#pragma once

#include <cstdint>
#include <string>

#include "Types.hpp"

namespace engine {

struct Config {
    std::string orderFile;
    std::string tradeFile;
    bool measureLatency = false;
    int cpuCore = -1;
    bool lockMemory = false;
    uint32_t genSeed = 42;
    std::size_t genCount = 100'000;
    uint8_t genCancelPercent = 20;
    Price genBasePrice = 10'000;
    double genMeanOffset = 14.0;
    double genStdDev = 8.0;
};

} // namespace engine
