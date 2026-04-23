#pragma once

#include <vector>
#include <string>

#include "Trade.hpp"

namespace engine {

class ResultWriter {
public:
    static void printSummary(double avgDurationMs,
                             double avgThroughput,
                             size_t ordersCnt,
                             size_t tradesCnt,
                             const std::vector<uint64_t>& latencies);
    static void writeTrades(const std::vector<Trade>& trades,
                            const std::string& filePath);

};

} // namespace engine
