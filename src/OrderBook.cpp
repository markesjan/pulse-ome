#include "OrderBook.hpp"

namespace engine {

OrderBook::OrderBook()
    : locations(&memoryPool), bids(&memoryPool), asks(&memoryPool) {
}

void OrderBook::insert(const Order& order) {
    if (order.side == Side::Buy) {
        locations[order.orderId] = {Side::Buy, order.price, bids[order.price].size()};
        bids[order.price].push_back(order);
    } else {
        locations[order.orderId] = {Side::Sell, order.price, asks[order.price].size()};
        asks[order.price].push_back(order);
    }
}

void OrderBook::cancel(const Order& order) {
    auto it = locations.find(order.orderId);
    if (it == locations.end())
        return;

    const auto& loc = it->second;

    Order& targetOrder = (loc.side == Side::Buy)
                             ? bids[loc.price][loc.vecIdx]
                             : asks[loc.price][loc.vecIdx];

    if (order.quantity >= targetOrder.quantity) {
        targetOrder.quantity = 0;
        locations.erase(it);
    } else {
        targetOrder.quantity -= order.quantity;
    }
}

void OrderBook::eraseLocation(OrderId orderId) {
    locations.erase(orderId);
}

Order* OrderBook::bestBid() {
    while (!bids.empty()) {
        auto levelIt = bids.begin();
        auto& orders = levelIt->second;

        for (auto& order : orders)
            if (order.quantity > 0)
                return &order;

        bids.erase(levelIt);
    }

    return nullptr;
}

Order* OrderBook::bestAsk() {
    while (!asks.empty()) {
        auto levelIt = asks.begin();
        auto& orders = levelIt->second;

        for (auto& order : orders)
            if (order.quantity > 0)
                return &order;

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
        OrderBookLevel currLevel;

        while (bidIt != bids.end()) {
            Quantity totalQty = 0;

            for (const auto& order : bidIt->second)
                totalQty += order.quantity;

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

            for (const auto& order : askIt->second)
                totalQty += order.quantity;

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

} // namespace engine
