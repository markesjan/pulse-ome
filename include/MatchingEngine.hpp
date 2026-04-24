#pragma once

#include <vector>

#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"

namespace engine {

class MatchingEngine {
public:
    void processOrder(const Order& order, std::vector<Trade>& trades);
    std::vector<OrderBookLevel> getBookState(std::size_t depth) const;

private:
    OrderBook lob;
    TradeId tradeIdCnt = 0;

    void processBuyOrder(const Order& order, std::vector<Trade>& trades);
    void processSellOrder(const Order& order, std::vector<Trade>& trades);
};

} // namespace engine
