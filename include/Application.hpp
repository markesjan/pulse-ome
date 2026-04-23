#pragma once

#include <memory>

#include "Config.hpp"
#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"

namespace engine {

class Application {
public:
    explicit Application(const Config& config);
    void run();

private:
    Config config;
    std::unique_ptr<OrderBook> orderBook;

    std::vector<Order> orders;
    std::vector<Trade> trades;
    std::vector<uint64_t> latencies;

    void prepareOrders();
    double matchOrders();
    static double getCyclesPerNs();
};

} // namespace engine
