#include "MatchingEngine.hpp"

namespace engine {

void MatchingEngine::processOrder(const Order& order, std::vector<Trade>& trades) {
    switch (order.eventType) {
        case EventType::New:
            order.side == Side::Buy
                ? processBuyOrder(order, trades)
                : processSellOrder(order, trades);
            break;

        case EventType::PartialCancel:
        case EventType::FullCancel:
            order.side == Side::Buy
                ? orderBook.cancelBuyOrder(order)
                : orderBook.cancelSellOrder(order);
            break;

        default:
            break;
    }
}

void MatchingEngine::processBuyOrder(const Order& order, std::vector<Trade>& trades) {
    Order currOrder = order;

    while (currOrder.quantity > 0) {
        Order* bestAsk = orderBook.getBestAsk();

        if (!bestAsk || currOrder.price < bestAsk->price)
            break;

        uint32_t tradeQty = std::min(currOrder.quantity, bestAsk->quantity);

        trades.emplace_back(Trade{
            currOrder.timestamp,
            tradeIdCnt++,
            currOrder.orderId,
            bestAsk->orderId,
            bestAsk->price,
            tradeQty
        });

        currOrder.quantity -= tradeQty;
        bestAsk->quantity -= tradeQty;

        if (bestAsk->quantity == 0)
            orderBook.popBestAsk();
    }

    if (currOrder.quantity > 0)
        orderBook.addBid(currOrder);
}

void MatchingEngine::processSellOrder(const Order& order, std::vector<Trade>& trades) {
    Order currOrder = order;

    while (currOrder.quantity > 0) {
        Order* bestBid = orderBook.getBestBid();

        if (!bestBid || currOrder.price > bestBid->price)
            break;

        uint32_t tradeQty = std::min(currOrder.quantity, bestBid->quantity);

        trades.emplace_back(Trade{
            currOrder.timestamp,
            tradeIdCnt++,
            bestBid->orderId,
            currOrder.orderId,
            bestBid->price,
            tradeQty
        });

        currOrder.quantity -= tradeQty;
        bestBid->quantity -= tradeQty;

        if (bestBid->quantity == 0)
            orderBook.popBestBid();
    }

    if (currOrder.quantity > 0)
        orderBook.addAsk(currOrder);
}

std::vector<OrderBookLevel> MatchingEngine::getBookState(std::size_t depth) const {
    return orderBook.getBookState(depth);
}

} // namespace engine
