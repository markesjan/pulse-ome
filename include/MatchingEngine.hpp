#pragma once

#include <vector>

#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"

namespace engine {

class MatchingEngine {
public:
    MatchingEngine(std::size_t maxOrders, Price maxPrice);
    void processOrder(const OrderMessage& orderMsg, std::vector<Trade>& trades);
    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    OrderBook orderBook;
    TradeId tradeIdCnt;

    void processBuyOrder(const OrderMessage& orderMsg, std::vector<Trade>& trades);
    void processSellOrder(const OrderMessage& orderMsg, std::vector<Trade>& trades);
};

} // namespace engine
