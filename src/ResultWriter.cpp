#include "ResultWriter.hpp"

#include <cstdio>
#include <fstream>
#include <numeric>
#include <print>

namespace engine {

void ResultWriter::printSummary(double avgDurationMs,
                                double avgThroughput,
                                std::size_t ordersCnt,
                                std::size_t tradesCnt,
                                const std::vector<uint64_t>& latencies) {
    std::println("\n============= Benchmark results =============");

    std::println("--- General ---");
    std::println("{:<20} {}", "Processed orders:", ordersCnt);
    std::println("{:<20} {}", "Created trades:", tradesCnt);

    if (ordersCnt > 0) {
        double matchRate = static_cast<double>(tradesCnt) / static_cast<double>(ordersCnt) * 100.0;
        std::println("{:<20} {:.2f} %", "Match rate:", matchRate);
    }

    std::println("{:<20} {:.2f} ms", "Avg duration:", avgDurationMs);
    std::println("{:<20} {:.2f} M orders/s", "Avg throughput:", avgThroughput / 1'000'000.0);

    if (latencies.empty()) {
        std::println("\n--- Latency ---\nLatency measurement disabled");
        std::println("=============================================");
        return;
    }

    double sum = std::accumulate(latencies.begin(), latencies.end(), 0.0);
    std::size_t n = latencies.size();
    auto dblN = static_cast<double>(n);

    std::println("\n--- Latency ---");
    std::println("{:<20} {} ns", "Min:", latencies.front());
    std::println("{:<20} {} ns", "Max:", latencies.back());
    std::println("{:<20} {} ns", "Average:", static_cast<std::size_t>(sum / dblN));
    std::println("{:<20} {} ns", "Median (p50):",
                 latencies[static_cast<std::size_t>(dblN * 0.50)]);
    std::println("{:<20} {} ns", "90. perc (p90):",
                 latencies[static_cast<std::size_t>(dblN * 0.90)]);
    std::println("{:<20} {} ns", "99. perc (p99):",
                 latencies[static_cast<std::size_t>(dblN * 0.99)]);
    std::println("{:<20} {} ns", "99.9. perc (p99.9):",
                 latencies[static_cast<std::size_t>(dblN * 0.999)]);

    std::println("=============================================");
}

void ResultWriter::writeTrades(const std::vector<Trade>& trades,
                               const std::string& filePath,
                               const OrderIdMapper* idMapper) {
    std::ofstream outFile(filePath);

    if (!outFile.is_open()) {
        std::println(stderr, "Couldn't open output file: {}", filePath);
        return;
    }

    std::println(outFile, "Timestamp,TradeID,BuyOrderID,SellOrderID,Quantity,Price");

    for (const auto& trade : trades) {
        OrderId externalBuyId = idMapper ? idMapper->toExternal(trade.buyOrderId) : trade.buyOrderId;
        OrderId externalSellId = idMapper ? idMapper->toExternal(trade.sellOrderId) : trade.sellOrderId;

        std::println(outFile, "{},{},{},{},{},{}",
                     trade.timestamp,
                     trade.tradeId,
                     externalBuyId,
                     externalSellId,
                     trade.quantity,
                     trade.price);
    }
}

} // namespace engine
