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
    std::map<Price, std::list<std::shared_ptr<Order>>, std::greater<>> bids;
    std::map<Price, std::list<std::shared_ptr<Order>>> asks;

    TradeId tradeIdCnt = 0;

    void processBuyOrder(const Order& order, std::vector<Trade>& trades);
    void processSellOrder(const Order& order, std::vector<Trade>& trades);
    void cancelBuyOrder(const Order& order);
    void cancelSellOrder(const Order& order);
};

} // namespace engine
