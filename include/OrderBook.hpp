#pragma once

#include <map>
#include <memory>
#include <memory_resource>
#include <unordered_map>
#include <vector>

#include "Order.hpp"
#include "OrderBookLevel.hpp"
#include "OrderLocation.hpp"

namespace engine {

class OrderBook {
public:
    OrderBook();

    void insert(const Order& order);
    void cancel(const Order& order);

    void eraseLocation(OrderId orderId);
    Order* bestBid();
    Order* bestAsk();

    std::vector<OrderBookLevel> bookState(std::size_t depth) const;

private:
    std::unique_ptr<std::byte[]> preallocBuffer;
    std::pmr::monotonic_buffer_resource memoryPool;

    using PmrOrderVector = std::pmr::vector<Order>;
    using PmrBidsMap = std::pmr::map<Price, PmrOrderVector, std::greater<>>;
    using PmrAsksMap = std::pmr::map<Price, PmrOrderVector>;
    using PmrLocationsUmap = std::pmr::unordered_map<OrderId, OrderLocation>;

    PmrLocationsUmap locations;
    PmrBidsMap bids;
    PmrAsksMap asks;
};

} // namespace engine
