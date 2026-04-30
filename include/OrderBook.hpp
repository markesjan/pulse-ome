#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Order.hpp"
#include "OrderBookLevel.hpp"
#include "OrderLocation.hpp"

namespace engine {

class OrderBook {
public:
    void insert(const Order& order);
    void cancel(const Order& order);

    void eraseLocation(OrderId orderId);
    Order* bestBid();
    Order* bestAsk();

    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    std::unordered_map<OrderId, OrderLocation> locations;

    std::map<Price, std::vector<Order>, std::greater<>> bids;
    std::map<Price, std::vector<Order>> asks;
};

} // namespace engine
