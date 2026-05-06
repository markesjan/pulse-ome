#include "MatchingEngine.hpp"

namespace engine {

MatchingEngine::MatchingEngine(std::size_t maxOrders, Price maxPrice)
    : orderBook(maxOrders, maxPrice), tradeIdCnt(0) {
}

void MatchingEngine::processOrder(const OrderMessage& orderMsg, std::vector<Trade>& trades) {
    switch (orderMsg.eventType) {
        case EventType::New:
            orderMsg.side == Side::Buy
                ? processBuyOrder(orderMsg, trades)
                : processSellOrder(orderMsg, trades);
            break;

        case EventType::PartialCancel:
            orderBook.cancelPartial(orderMsg.orderId, orderMsg.quantity);
            break;

        case EventType::FullCancel:
            orderBook.cancelFull(orderMsg.orderId);
            break;

        default:
            break;
    }
}

void MatchingEngine::processBuyOrder(const OrderMessage& orderMsg, std::vector<Trade>& trades) {
    Quantity remainingQty = orderMsg.quantity;

    while (remainingQty > 0) {
        Order* bestAsk = orderBook.bestAsk();

        if (!bestAsk || orderMsg.price < bestAsk->price)
            break;

        Quantity tradeQty = std::min(remainingQty, bestAsk->quantity);

        trades.emplace_back(Trade{
            orderMsg.timestamp,
            tradeIdCnt++,
            orderMsg.orderId,
            bestAsk->orderId,
            bestAsk->price,
            tradeQty
        });

        remainingQty -= tradeQty;
        bestAsk->quantity -= tradeQty;

        if (bestAsk->quantity == 0)
            orderBook.eraseLocation(bestAsk->orderId);
    }

    if (remainingQty > 0) {
        orderBook.insert(Order{
                             .timestamp = orderMsg.timestamp,
                             .orderId = orderMsg.orderId,
                             .price = orderMsg.price,
                             .quantity = remainingQty,
                             .nextOrderIdx = INVALID_INDEX
                         }, Side::Buy);
    }
}

void MatchingEngine::processSellOrder(const OrderMessage& orderMsg, std::vector<Trade>& trades) {
    Quantity remainingQty = orderMsg.quantity;

    while (remainingQty > 0) {
        Order* bestBid = orderBook.bestBid();

        if (!bestBid || orderMsg.price > bestBid->price)
            break;

        Quantity tradeQty = std::min(remainingQty, bestBid->quantity);

        trades.emplace_back(Trade{
            orderMsg.timestamp,
            tradeIdCnt++,
            bestBid->orderId,
            orderMsg.orderId,
            bestBid->price,
            tradeQty
        });

        remainingQty -= tradeQty;
        bestBid->quantity -= tradeQty;

        if (bestBid->quantity == 0)
            orderBook.eraseLocation(bestBid->orderId);
    }

    if (remainingQty > 0) {
        orderBook.insert(Order{
                             .timestamp = orderMsg.timestamp,
                             .orderId = orderMsg.orderId,
                             .price = orderMsg.price,
                             .quantity = remainingQty,
                             .nextOrderIdx = INVALID_INDEX
                         }, Side::Sell);
    }
}

std::vector<OrderBookLevel> MatchingEngine::bookState(std::size_t depth) const {
    return orderBook.bookState(depth);
}

} // namespace engine
