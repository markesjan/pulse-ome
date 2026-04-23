#include "OrderBook.hpp"

namespace engine {

void OrderBook::processOrder(const Order& order, std::vector<Trade>& trades) {
    switch (order.eventType) {
        case EventType::New:
            if (order.side == Side::Buy)
                processBuyOrder(order, trades);
            else
                processSellOrder(order, trades);
            break;

        case EventType::PartialCancel:
        case EventType::FullCancel:
            if (order.side == Side::Buy)
                cancelBuyOrder(order);
            else
                cancelSellOrder(order);
            break;

        default:
            break;
    }
}

void OrderBook::processBuyOrder(const Order& order, std::vector<Trade>& trades) {
    Order currOrder = order;

    auto it = asks.begin();
    while (it != asks.end() && currOrder.quantity > 0 && currOrder.price >= it->first) {
        auto& orderList = it->second;
        auto listIt = orderList.begin();

        while (listIt != orderList.end() && currOrder.quantity > 0) {
            auto sellOrder = *listIt;
            uint32_t tradeQty = std::min(currOrder.quantity, sellOrder->quantity);

            trades.emplace_back(Trade{
                currOrder.timestamp,
                tradeIdCnt++,
                currOrder.orderId,
                sellOrder->orderId,
                sellOrder->price,
                tradeQty,
            });

            currOrder.quantity -= tradeQty;
            sellOrder->quantity -= tradeQty;

            if (sellOrder->quantity == 0)
                listIt = orderList.erase(listIt);
            else
                ++listIt;
        }

        if (orderList.empty())
            it = asks.erase(it);
        else
            ++it;
    }

    if (currOrder.quantity > 0)
        bids[currOrder.price].push_back(std::make_shared<Order>(currOrder));
}

void OrderBook::processSellOrder(const Order& order, std::vector<Trade>& trades) {
    Order currOrder = order;

    auto it = bids.begin();
    while (it != bids.end() && currOrder.quantity > 0 && currOrder.price <= it->first) {
        auto& orderList = it->second;
        auto listIt = orderList.begin();

        while (listIt != orderList.end() && currOrder.quantity > 0) {
            auto buyOrder = *listIt;
            uint32_t tradeQty = std::min(currOrder.quantity, buyOrder->quantity);

            trades.emplace_back(Trade{
                currOrder.timestamp,
                tradeIdCnt++,
                buyOrder->orderId,
                currOrder.orderId,
                buyOrder->price,
                tradeQty,
            });

            currOrder.quantity -= tradeQty;
            buyOrder->quantity -= tradeQty;

            if (buyOrder->quantity == 0)
                listIt = orderList.erase(listIt);
            else
                ++listIt;
        }

        if (orderList.empty())
            it = bids.erase(it);
        else
            ++it;
    }

    if (currOrder.quantity > 0)
        asks[currOrder.price].push_back(std::make_shared<Order>(currOrder));
}

void OrderBook::cancelBuyOrder(const Order& order) {
    for (auto mapIt = bids.begin(); mapIt != bids.end(); ++mapIt) {
        auto& orderList = mapIt->second;

        for (auto listIt = orderList.begin(); listIt != orderList.end(); ++listIt) {
            if ((*listIt)->orderId == order.orderId) {
                if (order.eventType == EventType::FullCancel)
                    orderList.erase(listIt);
                else if (order.eventType == EventType::PartialCancel) {
                    if ((*listIt)->quantity <= order.quantity)
                        orderList.erase(listIt);
                    else
                        (*listIt)->quantity -= order.quantity;
                }

                if (orderList.empty())
                    bids.erase(mapIt);

                return;
            }
        }
    }
}

void OrderBook::cancelSellOrder(const Order& order) {
    for (auto mapIt = asks.begin(); mapIt != asks.end(); ++mapIt) {
        auto& orderList = mapIt->second;

        for (auto listIt = orderList.begin(); listIt != orderList.end(); ++listIt) {
            if ((*listIt)->orderId == order.orderId) {
                if (order.eventType == EventType::FullCancel)
                    orderList.erase(listIt);
                else if (order.eventType == EventType::PartialCancel) {
                    if ((*listIt)->quantity <= order.quantity)
                        orderList.erase(listIt);
                    else
                        (*listIt)->quantity -= order.quantity;
                }

                if (orderList.empty())
                    asks.erase(mapIt);

                return;
            }
        }
    }
}

} // namespace engine
