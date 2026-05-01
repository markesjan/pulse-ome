#include "OrderBook.hpp"

#include <algorithm>

namespace engine {

OrderBook::OrderBook(std::size_t maxOrders) {
    ordersPool.reserve(maxOrders);
    freeIndices.reserve(maxOrders);
}

void OrderBook::insert(const Order& order) {
    OrderIndex idx = allocateOrder(order);
    locations[order.orderId] = idx;

    order.side == Side::Buy
        ? bids[order.price].push_back(idx)
        : asks[order.price].push_back(idx);
}

void OrderBook::cancel(const Order& order) {
    auto locIt = locations.find(order.orderId);
    if (locIt == locations.end())
        return;

    OrderIndex idx = locIt->second;

    Order& poolOrder = ordersPool[idx];

    if (order.eventType == EventType::PartialCancel) {
        if (poolOrder.quantity > order.quantity) {
            poolOrder.quantity -= order.quantity;
            return;
        }
    }

    poolOrder.quantity = 0;
    locations.erase(locIt);
}

void OrderBook::eraseLocation(OrderId orderId) {
    locations.erase(orderId);
}

Order* OrderBook::bestBid() {
    while (!bids.empty()) {
        auto levelIt = bids.begin();
        auto& level = levelIt->second;

        while (!level.empty()) {
            OrderIndex frontIdx = level.front();

            if (ordersPool[frontIdx].quantity == 0) {
                freeOrder(frontIdx);
                level.erase(level.begin());
            } else {
                return &ordersPool[frontIdx];
            }
        }

        bids.erase(levelIt);
    }

    return nullptr;
}

Order* OrderBook::bestAsk() {
    while (!asks.empty()) {
        auto levelIt = asks.begin();
        auto& level = levelIt->second;

        while (!level.empty()) {
            OrderIndex frontIdx = level.front();

            if (ordersPool[frontIdx].quantity == 0) {
                freeOrder(frontIdx);
                level.erase(level.begin());
            } else {
                return &ordersPool[frontIdx];
            }
        }

        asks.erase(levelIt);
    }

    return nullptr;
}

std::vector<OrderBookLevel> OrderBook::bookState(std::size_t depth) const {
    if (depth == 0)
        return {};

    std::vector<OrderBookLevel> state;
    state.reserve(depth);

    auto bidIt = bids.begin();
    auto askIt = asks.begin();

    for (std::size_t i = 0; i < depth; ++i) {
        OrderBookLevel currLevel{};

        while (bidIt != bids.end()) {
            Quantity totalQty = 0;
            for (const auto& index : bidIt->second)
                totalQty += ordersPool[index].quantity;

            if (totalQty > 0) {
                currLevel.bidPrice = bidIt->first;
                currLevel.bidQty = totalQty;
                ++bidIt;
                break;
            }
            ++bidIt;
        }

        while (askIt != asks.end()) {
            Quantity totalQty = 0;
            for (const auto& index : askIt->second)
                totalQty += ordersPool[index].quantity;

            if (totalQty > 0) {
                currLevel.askPrice = askIt->first;
                currLevel.askQty = totalQty;
                ++askIt;
                break;
            }
            ++askIt;
        }

        state.push_back(currLevel);
    }

    return state;
}

OrderIndex OrderBook::allocateOrder(const Order& order) {
    if (!freeIndices.empty()) {
        OrderIndex idx = freeIndices.back();
        freeIndices.pop_back();
        ordersPool[idx] = order;
        return idx;
    }

    if (ordersPool.size() >= ordersPool.capacity())
        throw std::runtime_error("OrderBook memory pool exhausted!");

    ordersPool.push_back(order);
    return static_cast<OrderIndex>(ordersPool.size() - 1);
}

void OrderBook::freeOrder(OrderIndex index) {
    freeIndices.push_back(index);
}

} // namespace engine
