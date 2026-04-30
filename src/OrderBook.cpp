#include "OrderBook.hpp"

namespace engine {

void OrderBook::insert(const Order& order) {
    if (order.side == Side::Buy)
        bids[order.price].push_back(order);
    else
        asks[order.price].push_back(order);
}

void OrderBook::removeBestBid() {
    if (bids.empty())
        return;

    auto it = bids.begin();
    it->second.pop_front();

    if (it->second.empty())
        bids.erase(it);
}

void OrderBook::removeBestAsk() {
    if (asks.empty())
        return;

    auto it = asks.begin();
    it->second.pop_front();

    if (it->second.empty())
        asks.erase(it);
}

void OrderBook::cancel(const Order& order) {
    if (order.side == Side::Buy) {
        for (auto mapIt = bids.begin(); mapIt != bids.end(); ++mapIt) {
            auto& orderList = mapIt->second;

            for (auto listIt = orderList.begin(); listIt != orderList.end(); ++listIt) {
                if (listIt->orderId == order.orderId) {
                    if (order.eventType == EventType::FullCancel)
                        orderList.erase(listIt);
                    else if (order.eventType == EventType::PartialCancel) {
                        if (listIt->quantity <= order.quantity)
                            orderList.erase(listIt);
                        else
                            listIt->quantity -= order.quantity;
                    }

                    if (orderList.empty())
                        bids.erase(mapIt);

                    return;
                }
            }
        }
    } else {
        for (auto mapIt = asks.begin(); mapIt != asks.end(); ++mapIt) {
            auto& orderList = mapIt->second;

            for (auto listIt = orderList.begin(); listIt != orderList.end(); ++listIt) {
                if (listIt->orderId == order.orderId) {
                    if (order.eventType == EventType::FullCancel)
                        orderList.erase(listIt);
                    else if (order.eventType == EventType::PartialCancel) {
                        if (listIt->quantity <= order.quantity)
                            orderList.erase(listIt);
                        else
                            listIt->quantity -= order.quantity;
                    }

                    if (orderList.empty())
                        asks.erase(mapIt);

                    return;
                }
            }
        }
    }
}

Order* OrderBook::bestBid() {
    if (bids.empty())
        return nullptr;

    return &bids.begin()->second.front();
}

Order* OrderBook::bestAsk() {
    if (asks.empty())
        return nullptr;

    return &asks.begin()->second.front();
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

        if (bidIt != bids.end()) {
            currLevel.bidPrice = bidIt->first;

            for (const auto& order : bidIt->second)
                currLevel.bidQty += order.quantity;

            ++bidIt;
        }

        if (askIt != asks.end()) {
            currLevel.askPrice = askIt->first;

            for (const auto& order : askIt->second)
                currLevel.askQty += order.quantity;

            ++askIt;
        }

        state.push_back(currLevel);
    }

    return state;
}

} // namespace engine
