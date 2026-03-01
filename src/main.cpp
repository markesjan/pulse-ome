#include <chrono>
#include <iostream>

#include "Order.hpp"
#include "DataLoader.hpp"
#include "MatchingEngine.hpp"
#include "ResultWriter.hpp"
#include "Trade.hpp"

int main() {
    std::string fileName = "../data/AAPL_orders_lvl5.csv";
    std::cout << "Loading orders from file: " << fileName << std::endl;
    std::vector<engine::Order> orders = engine::DataLoader::loadOrders(fileName);

    if (orders.empty()) {
        std::cerr << "Couldn't load any data" << std::endl;
        return 1;
    }

    engine::MatchingEngine matchEngine;
    std::vector<engine::Trade> allTrades;

    std::cout << "Starting matching orders" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &order : orders) {
        std::vector<engine::Trade> newTrades = matchEngine.processOrder(order);

        if (!newTrades.empty())
            allTrades.insert(allTrades.end(), newTrades.begin(), newTrades.end());
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "All orders were processed in: " << duration.count() << "ms" << std::endl;
    std::cout << "Total trades generated: " << allTrades.size() << std::endl;

    engine::ResultWriter::writeTrades(allTrades, "../output/trades.csv");

    return 0;
}
