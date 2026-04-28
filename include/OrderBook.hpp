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
    void insert(const Order& order);
    void cancel(const Order& order);

    void removeBestBid();
    void removeBestAsk();
    Order* bestBid();
    Order* bestAsk();

    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    std::map<Price, std::list<Order>, std::greater<>> bids;
    std::map<Price, std::list<Order>> asks;
};

} // namespace engine
