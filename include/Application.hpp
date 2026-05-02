#pragma once

#include <optional>

#include "Config.hpp"
#include "Order.hpp"
#include "MatchingEngine.hpp"
#include "OrderIdMapper.hpp"
#include "Trade.hpp"

namespace engine {

class Application {
public:
    explicit Application(const Config& config);
    void run();

private:
    Config config;
    std::optional<OrderIdMapper> idMapper;
    std::optional<MatchingEngine> matchingEngine;

    std::vector<Order> orders;
    std::vector<Trade> trades;
    std::vector<uint64_t> latencies;

    void prepareOrders();
    double matchOrders();
    static double getCyclesPerNs();
};

} // namespace engine
