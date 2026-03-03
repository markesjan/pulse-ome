#include "ResultWriter.hpp"

#include <fstream>
#include <iostream>

namespace engine {

void ResultWriter::printSummary(double totalTimeSec, size_t ordersCnt, size_t tradesCnt, std::vector<long long> latencies) {
    std::cout << "\n==== Benchmark results ====" << std::endl;
    std::cout << "Total duration: " << totalTimeSec << " s" << std::endl;
    std::cout << "Throughput:  " << static_cast<long long>(ordersCnt / totalTimeSec) << " orders/s" << std::endl;

    double sum = 0;
    for (auto l : latencies) sum += l;

    std::cout << "\n--- Latency ---" << std::endl;
    std::cout << "Average:            " << (sum / latencies.size()) << " ns" << std::endl;
    std::cout << "Median (p50):       " << latencies[latencies.size() * 0.50] << " ns" << std::endl;
    std::cout << "90. perc (p90):     " << latencies[latencies.size() * 0.90] << " ns" << std::endl;
    std::cout << "99. perc (p99):     " << latencies[latencies.size() * 0.99] << " ns" << std::endl;
    std::cout << "99.9. perc (p99.9): " << latencies[latencies.size() * 0.999] << " ns" << std::endl;
    std::cout << "Max latency: " << latencies.back() << " ns" << std::endl;
    std::cout << "===========================\n" << std::endl;
}

void ResultWriter::writeTrades(const std::vector<Trade>& trades, const std::string& filePath) {
    std::ofstream outFile(filePath);

    if (!outFile.is_open()) {
        std::cerr << "Couldn't open output file: " << filePath << std::endl;
        return;
    }

    outFile << "Symbol,BuyOrderID,SellOrderID,Quantity,Price\n";

    for (const auto& trade : trades) {
        outFile << trade.symbol << ","
            << trade.buyOrderId << ","
            << trade.sellOrderId << ","
            << trade.quantity << ","
            << trade.price << "\n";
    }
}

} // namespace engine
