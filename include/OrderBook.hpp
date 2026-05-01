#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Order.hpp"
#include "OrderBookLevel.hpp"

namespace engine {

class OrderBook {
public:
    explicit OrderBook(std::size_t maxOrders = 10'000'000);

    void insert(const Order& order);
    void cancel(const Order& order);

    void eraseLocation(OrderId orderId);
    Order* bestBid();
    Order* bestAsk();

    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    std::vector<Order> ordersPool;
    std::vector<OrderIndex> freeIndices;

    std::map<Price, std::vector<OrderIndex>, std::greater<>> bids;
    std::map<Price, std::vector<OrderIndex>> asks;

    std::unordered_map<OrderId, OrderIndex> locations;

    OrderIndex allocateOrder(const Order& order);
    void freeOrder(OrderIndex index);
};

} // namespace engine
