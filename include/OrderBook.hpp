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
    void processOrder(const Order& order, std::vector<Trade>& trades);

private:
    std::map<uint64_t, std::list<std::shared_ptr<Order>>, std::greater<uint64_t>> buys;
    std::map<uint64_t, std::list<std::shared_ptr<Order>>> sells;

    uint64_t tradeIdCnt = 0;

    void processBuyOrder(const Order& order, std::vector<Trade>& trades);
    void processSellOrder(const Order& order, std::vector<Trade>& trades);
    void cancelBuyOrder(const Order& order);
    void cancelSellOrder(const Order& order);
};

} // namespace engine
