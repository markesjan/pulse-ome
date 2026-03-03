#include <chrono>
#include <iostream>

#include "ConfigParser.hpp"
#include "Order.hpp"
#include "DataLoader.hpp"
#include "MatchingEngine.hpp"
#include "ResultWriter.hpp"
#include "Trade.hpp"

int main(int argc, char* argv[]) {
    engine::Config config = engine::ConfigParser::parseArguments(argc, argv);
    std::cout << "Loading orders from file: " << config.inFile << std::endl;
    std::vector<engine::Order> orders = engine::DataLoader::loadOrders(config.inFile);

    if (orders.empty()) {
        std::cerr << "Couldn't load any data" << std::endl;
        return 1;
    }

    engine::MatchingEngine matchEngine;
    std::vector<engine::Trade> allTrades;
    std::vector<long long> latencies;

    std::cout << "Started matching orders..." << std::endl;
    auto totalStart = std::chrono::high_resolution_clock::now();

    for (const auto& order : orders) {
        auto tickStart = std::chrono::high_resolution_clock::now();
        std::vector<engine::Trade> newTrades = matchEngine.processOrder(order);
        auto tickEnd = std::chrono::high_resolution_clock::now();

        auto durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(tickEnd - tickStart).
            count();
        latencies.push_back(durationNs);

        if (!newTrades.empty())
            allTrades.insert(allTrades.end(), newTrades.begin(), newTrades.end());
    }

    auto totalEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = totalEnd - totalStart;
    std::sort(latencies.begin(), latencies.end());

    engine::ResultWriter::printSummary(totalDuration.count(), orders.size(), allTrades.size(),
                                       latencies);
    engine::ResultWriter::writeTrades(allTrades, config.outFile);

    return 0;
}
