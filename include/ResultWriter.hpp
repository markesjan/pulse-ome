/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  ResultWriter.hpp
 * @brief Declaration of the ResultWriter class for printing and saving results.
 */

#pragma once

#include <vector>
#include <string>

#include "OrderIdMapper.hpp"
#include "Trade.hpp"

namespace engine {

class ResultWriter {
public:
    static void printSummary(double avgDurationMs,
                             double avgThroughput,
                             std::size_t ordersCnt,
                             std::size_t tradesCnt,
                             const std::vector<uint64_t>& latencies);
    static void writeTrades(const std::vector<Trade>& trades,
                            const std::string& filePath,
                            const OrderIdMapper* idMapper = nullptr);

};

} // namespace engine
