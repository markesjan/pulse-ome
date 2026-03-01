#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "Order.hpp"
#include "Trade.hpp"

namespace engine {

class OrderBook {
public:
    std::vector<Trade> processOrder(const Order& order);

private:
    std::map<uint64_t, std::list<std::shared_ptr<Order>>, std::greater<uint64_t>> buys;
    std::map<uint64_t, std::list<std::shared_ptr<Order>>> sells;

    std::vector<Trade> processBuyOrder(const Order& order);
    std::vector<Trade> processSellOrder(const Order& order);
    void cancelBuyOrder(const Order& order);
    void cancelSellOrder(const Order& order);
};

} // namespace engine
