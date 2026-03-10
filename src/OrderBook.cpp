#include "OrderBook.hpp"

namespace engine {

void OrderBook::processOrder(const Order& order, std::vector<Trade>& trades) {
    if (order.eventType == 2 || order.eventType == 3) {
        order.side == 1 ? cancelBuyOrder(order) : cancelSellOrder(order);
        return;
    }

    if (order.eventType != 1)
        return;

    order.side == 1 ? processBuyOrder(order, trades) : processSellOrder(order, trades);
}

void OrderBook::processBuyOrder(const Order& order, std::vector<Trade>& trades) {
    Order currOrder = order;

    auto it = sells.begin();
    while (it != sells.end() && currOrder.quantity > 0 && currOrder.price >= it->first) {
        auto& orderList = it->second;
        auto listIt = orderList.begin();

        while (listIt != orderList.end() && currOrder.quantity > 0) {
            auto sellOrder = *listIt;
            uint32_t tradeQty = std::min(currOrder.quantity, sellOrder->quantity);

            trades.emplace_back(Trade{
                currOrder.symbol,
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
            it = sells.erase(it);
        else
            ++it;
    }

    if (currOrder.quantity > 0)
        buys[currOrder.price].push_back(std::make_shared<Order>(currOrder));
}

void OrderBook::processSellOrder(const Order& order, std::vector<Trade>& trades) {
    Order currOrder = order;

    auto it = buys.begin();
    while (it != buys.end() && currOrder.quantity > 0 && currOrder.price <= it->first) {
        auto& orderList = it->second;
        auto listIt = orderList.begin();

        while (listIt != orderList.end() && currOrder.quantity > 0) {
            auto buyOrder = *listIt;
            uint32_t tradeQty = std::min(currOrder.quantity, buyOrder->quantity);

            trades.emplace_back(Trade{
                currOrder.symbol,
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
            it = buys.erase(it);
        else
            ++it;
    }

    if (currOrder.quantity > 0)
        sells[currOrder.price].push_back(std::make_shared<Order>(currOrder));
}

void OrderBook::cancelBuyOrder(const Order& order) {
    for (auto mapIt = buys.begin(); mapIt != buys.end(); ++mapIt) {
        auto& orderList = mapIt->second;

        for (auto listIt = orderList.begin(); listIt != orderList.end(); ++listIt) {
            if ((*listIt)->orderId == order.orderId) {
                if (order.eventType == 3)
                    orderList.erase(listIt);
                else if (order.eventType == 2) {
                    if ((*listIt)->quantity <= order.quantity)
                        orderList.erase(listIt);
                    else
                        (*listIt)->quantity -= order.quantity;
                }

                if (orderList.empty())
                    buys.erase(mapIt);

                return;
            }
        }
    }
}

void OrderBook::cancelSellOrder(const Order& order) {
    for (auto mapIt = sells.begin(); mapIt != sells.end(); ++mapIt) {
        auto& orderList = mapIt->second;

        for (auto listIt = orderList.begin(); listIt != orderList.end(); ++listIt) {
            if ((*listIt)->orderId == order.orderId) {
                if (order.eventType == 3)
                    orderList.erase(listIt);
                else if (order.eventType == 2) {
                    if ((*listIt)->quantity <= order.quantity)
                        orderList.erase(listIt);
                    else
                        (*listIt)->quantity -= order.quantity;
                }

                if (orderList.empty())
                    sells.erase(mapIt);

                return;
            }
        }
    }
}

} // namespace engine
