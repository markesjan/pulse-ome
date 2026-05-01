#include "OrderBook.hpp"

#include <algorithm>
#include <stdexcept>

namespace engine {

OrderBook::OrderBook(std::size_t maxOrders, Price maxPrice) {
    ordersPool.reserve(maxOrders);
    freeIndices.reserve(maxOrders);

    bids.resize(maxPrice + 1);
    asks.resize(maxPrice + 1);

    locations.resize(maxOrders, INVALID_INDEX);
}

void OrderBook::insert(const Order& order) {
    OrderIndex idx = allocateOrder(order);
    locations[order.orderId] = idx;

    if (order.side == Side::Buy) {
        bids[order.price].push_back(idx);

        if (order.price > currBestBid)
            currBestBid = order.price;
    } else {
        asks[order.price].push_back(idx);

        if (order.price < currBestAsk)
            currBestAsk = order.price;
    }
}

void OrderBook::cancel(const Order& order) {
    OrderIndex idx = locations[order.orderId];

    if (idx == INVALID_INDEX)
        return;

    Order& poolOrder = ordersPool[idx];

    if (order.eventType == EventType::PartialCancel) {
        if (poolOrder.quantity > order.quantity) {
            poolOrder.quantity -= order.quantity;
            return;
        }
    }

    poolOrder.quantity = 0;
    locations[order.orderId] = INVALID_INDEX;
}

void OrderBook::eraseLocation(OrderId orderId) {
    if (orderId < locations.size())
        locations[orderId] = INVALID_INDEX;
}

Order* OrderBook::bestBid() {
    while (currBestBid > 0) {
        std::vector<OrderIndex>& level = bids[currBestBid];

        while (!level.empty()) {
            OrderIndex frontIdx = level.front();

            if (ordersPool[frontIdx].quantity == 0) {
                freeOrder(frontIdx);
                level.erase(level.begin());
            } else {
                return &ordersPool[frontIdx];
            }
        }

        currBestBid--;
    }

    return nullptr;
}

Order* OrderBook::bestAsk() {
    while (currBestAsk < asks.size()) {
        std::vector<OrderIndex>& level = asks[currBestAsk];

        while (!level.empty()) {
            OrderIndex frontIdx = level.front();

            if (ordersPool[frontIdx].quantity == 0) {
                freeOrder(frontIdx);
                level.erase(level.begin());
            } else {
                return &ordersPool[frontIdx];
            }
        }

        currBestAsk++;
    }

    return nullptr;
}

std::vector<OrderBookLevel> OrderBook::bookState(std::size_t depth) const {
    if (depth == 0)
        return {};

    std::vector<OrderBookLevel> state;
    state.reserve(depth);

    Price scanBid = currBestBid;
    Price scanAsk = currBestAsk;

    for (std::size_t i = 0; i < depth; ++i) {
        OrderBookLevel currLevel{};
        bool foundBid = false;
        bool foundAsk = false;

        while (scanBid > 0 && !foundBid) {
            const std::vector<OrderIndex>& level = bids[scanBid];
            Quantity totalQty = 0;

            for (OrderIndex idx : level)
                totalQty += ordersPool[idx].quantity;

            if (totalQty > 0) {
                currLevel.bidPrice = scanBid;
                currLevel.bidQty = totalQty;
                foundBid = true;
            }
            scanBid--;
        }

        while (scanAsk < asks.size() && !foundAsk) {
            const std::vector<OrderIndex>& level = asks[scanAsk];
            Quantity totalQty = 0;

            for (OrderIndex idx : level)
                totalQty += ordersPool[idx].quantity;

            if (totalQty > 0) {
                currLevel.askPrice = scanAsk;
                currLevel.askQty = totalQty;
                foundAsk = true;
            }
            scanAsk++;
        }

        if (foundBid || foundAsk) {
            state.push_back(currLevel);
        } else {
            state.resize(depth);
            break;
        }
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
