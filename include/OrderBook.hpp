#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "Order.hpp"
#include "OrderBookLevel.hpp"

namespace engine {

class OrderBook {
public:
    void addBid(const Order& order);
    void addAsk(const Order& order);
    void popBestBid();
    void popBestAsk();
    void cancelBuyOrder(const Order& order);
    void cancelSellOrder(const Order& order);

    Order* getBestBid();
    Order* getBestAsk();

    std::vector<OrderBookLevel> getBookState(std::size_t depth) const;

private:
    std::map<Price, std::list<std::shared_ptr<Order>>, std::greater<>> bids;
    std::map<Price, std::list<std::shared_ptr<Order>>> asks;
};

} // namespace engine
