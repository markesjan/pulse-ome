#include "ResultWriter.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>

namespace engine {

void ResultWriter::printSummary(double totalTimeSec,
                                size_t ordersCnt,
                                size_t tradesCnt,
                                const std::vector<int64_t>& latencies) {
    std::cout << "\n========== Benchmark results ==========\n";

    std::cout << "--- General ---\n"
        << std::left << std::setw(20) << "Processed orders:" << ordersCnt << "\n"
        << std::left << std::setw(20) << "Created trades:" << tradesCnt << "\n"
        << std::left << std::setw(20) << "Total duration:" << totalTimeSec << " s\n";

    if (totalTimeSec > 0) {
        std::cout << std::left << std::setw(20) << "Throughput:"
            << static_cast<int64_t>(ordersCnt / totalTimeSec) << " orders/s\n";
    }

    if (latencies.empty()) {
        std::cout << "\nNo latency data available.\n";
        std::cout << "=======================================\n\n";
        return;
    }

    double sum = std::accumulate(latencies.begin(), latencies.end(), 0.0);
    size_t n = latencies.size();

    std::cout << "\n--- Latency ---\n"
        << std::left << std::setw(20) << "Min:" << latencies.front() << " ns\n"
        << std::left << std::setw(20) << "Max:" << latencies.back() << " ns\n"
        << std::left << std::setw(20) << "Average:" << (sum / n) << " ns\n"
        << std::left << std::setw(20) << "Median (p50):" << latencies[static_cast<size_t>(
            n * 0.50)] << " ns\n"
        << std::left << std::setw(20) << "90. perc (p90):" << latencies[static_cast<size_t>(
            n * 0.90)] << " ns\n"
        << std::left << std::setw(20) << "99. perc (p99):" << latencies[static_cast<size_t>(
            n * 0.99)] << " ns\n"
        << std::left << std::setw(20) << "99.9. perc (p99.9):" << latencies[static_cast<size_t>(
            n * 0.999)] << " ns\n";

    std::cout << "=======================================\n" << std::endl;
}

void ResultWriter::writeTrades(const std::vector<Trade>& trades, const std::string& filePath) {
    std::ofstream outFile(filePath);

    if (!outFile.is_open()) {
        std::cerr << "Couldn't open output file: " << filePath << std::endl;
        return;
    }

    outFile << "BuyOrderID,SellOrderID,Quantity,Price\n";

    for (const auto& trade : trades) {
        outFile << trade.buyOrderId << ","
            << trade.sellOrderId << ","
            << trade.quantity << ","
            << trade.price << "\n";
    }
}

} // namespace engine
