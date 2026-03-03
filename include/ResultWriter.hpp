#pragma once

#include <vector>

#include "Trade.hpp"

namespace engine {

class ResultWriter {
public:
    static void printSummary(double totalTimeSec, size_t ordersCnt, size_t tradesCnt, std::vector<long long> latencies);
    static void writeTrades(const std::vector<Trade>& trades, const std::string& filePath);
};

} // namespace engine
