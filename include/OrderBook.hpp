#pragma once

#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

#include "Order.hpp"
#include "OrderBookLevel.hpp"

namespace engine {

constexpr OrderIndex INVALID_INDEX = std::numeric_limits<OrderIndex>::max();
constexpr std::size_t MAX_ORDERS = 10'000'000;
constexpr Price MAX_PRICE = 10'000'000;

class OrderBook {
public:
    explicit OrderBook(std::size_t maxOrders = MAX_ORDERS, Price maxPrice = MAX_PRICE);

    void insert(const Order& order);
    void cancel(const Order& order);

    void eraseLocation(OrderId orderId);
    Order* bestBid();
    Order* bestAsk();

    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    std::vector<Order> ordersPool;
    std::vector<OrderIndex> freeIndices;

    std::vector<std::vector<OrderIndex>> bids;
    std::vector<std::vector<OrderIndex>> asks;

    std::vector<OrderIndex> locations;

    Price currBestBid = 0;
    Price currBestAsk = std::numeric_limits<Price>::max();

    OrderIndex allocateOrder(const Order& order);
    void freeOrder(OrderIndex index);
};

} // namespace engine
