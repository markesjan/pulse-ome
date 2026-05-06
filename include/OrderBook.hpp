#pragma once

#include <vector>

#include "Order.hpp"
#include "OrderBookLevel.hpp"
#include "PriceBitset.hpp"
#include "PriceLevel.hpp"

namespace engine {

class OrderBook {
public:
    OrderBook(std::size_t maxOrders, Price maxPrice);

    void insert(const Order& order, Side side);
    void cancelPartial(OrderId orderId, Quantity cancelQty);
    void cancelFull(OrderId orderId);

    void eraseLocation(OrderId orderId);
    Order* bestBid();
    Order* bestAsk();

    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    std::vector<Order> ordersPool;
    OrderIndex firstFreeIdx;
    std::vector<OrderIndex> locations;

    std::vector<PriceLevel> bids;
    std::vector<PriceLevel> asks;

    PriceBitset bidBits;
    PriceBitset askBits;

    Price currBestBid;
    Price currBestAsk;

    OrderIndex allocateOrder(const Order& order);
    void freeOrder(OrderIndex index);
};

} // namespace engine
