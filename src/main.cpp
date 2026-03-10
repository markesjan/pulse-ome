#include <chrono>
#include <iostream>
#include <algorithm>

#include "ConfigParser.hpp"
#include "Order.hpp"
#include "DataLoader.hpp"
#include "OrderBook.hpp"
#include "OrderGenerator.hpp"
#include "ResultWriter.hpp"
#include "Trade.hpp"

int main(int argc, char* argv[]) {
    engine::Config config = engine::ConfigParser::parseArguments(argc, argv);

    std::vector<engine::Order> orders;

    if (!config.inFile.empty()) {
        std::cout << "Loading orders from file: " << config.inFile << std::endl;
        orders = engine::DataLoader::loadOrders(config.inFile);
    } else {
        std::cout << "Generating " << config.generateCnt << " orders (seed: "
            << config.seed << ")" << std::endl;
        orders = engine::OrderGenerator::generate(config.seed, config.generateCnt);
    }

    if (orders.empty()) {
        std::cerr << "Couldn't load or generate any data" << std::endl;
        return 1;
    }

    engine::OrderBook orderBook;
    std::vector<engine::Trade> allTrades;
    std::vector<int64_t> latencies;

    std::cout << "Started matching orders..." << std::endl;
    auto matchingStart = std::chrono::high_resolution_clock::now();

    for (const auto& order : orders) {
        auto start = std::chrono::high_resolution_clock::now();
        orderBook.processOrder(order, allTrades);
        auto end = std::chrono::high_resolution_clock::now();

        auto durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        latencies.push_back(durationNs);
    }

    auto matchingEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = matchingEnd - matchingStart;
    std::ranges::sort(latencies);

    engine::ResultWriter::printSummary(totalDuration.count(), orders.size(), allTrades.size(),
                                       latencies);

    if (!config.outFile.empty())
        engine::ResultWriter::writeTrades(allTrades, config.outFile);

    return 0;
}
