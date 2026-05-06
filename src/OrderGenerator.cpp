#include "OrderGenerator.hpp"

#include <random>

namespace engine {

std::vector<OrderMessage> OrderGenerator::generate(uint32_t seed,
                                                   std::size_t ordersCount,
                                                   uint8_t cancelPercent,
                                                   Price basePrice,
                                                   double meanOffset,
                                                   double stdDev) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution eventDist(1, 100);
    std::uniform_int_distribution sideDist(0, 1);
    std::uniform_int_distribution quantityDist(1, 100);

    std::normal_distribution buyPriceDist(static_cast<double>(basePrice) - meanOffset, stdDev);
    std::normal_distribution sellPriceDist(static_cast<double>(basePrice) + meanOffset, stdDev);

    struct ActiveOrder {
        OrderId id;
        Side side;
    };

    std::vector<OrderMessage> orderMsgs;
    orderMsgs.reserve(ordersCount);
    std::vector<ActiveOrder> activeOrders;
    activeOrders.reserve(ordersCount);

    OrderId nextOrderId = 0;
    Timestamp currTimestamp = 1'000'000'000;

    for (std::size_t i = 0; i < ordersCount; ++i) {
        currTimestamp += 1'000'000;

        if (eventDist(gen) <= static_cast<int>(cancelPercent) && !activeOrders.empty()) {
            std::size_t window_size = std::min<std::size_t>(activeOrders.size(), 100);
            std::uniform_int_distribution recentDist(activeOrders.size() - window_size,
                                                     activeOrders.size() - 1);
            std::size_t cancelIdx = recentDist(gen);
            ActiveOrder orderToCancel = activeOrders[cancelIdx];

            std::uniform_int_distribution typeDist(2, 3);
            auto rawCancelType = typeDist(gen);
            auto cancelType = rawCancelType == 3 ? EventType::FullCancel : EventType::PartialCancel;

            Quantity cancelQty = 0;

            if (cancelType == EventType::FullCancel) {
                cancelQty = 0;
                activeOrders[cancelIdx] = activeOrders.back();
                activeOrders.pop_back();
            } else {
                std::uniform_int_distribution<uint32_t> partialQtyDist(1, 10);
                cancelQty = partialQtyDist(gen);
            }

            orderMsgs.push_back(OrderMessage{
                currTimestamp,
                orderToCancel.id,
                0,
                cancelQty,
                cancelType,
                orderToCancel.side
            });
        } else {
            auto side = sideDist(gen) == 0 ? Side::Buy : Side::Sell;

            double rawPrice = side == Side::Buy ? buyPriceDist(gen) : sellPriceDist(gen);
            auto price = static_cast<Price>(std::max(1.0, rawPrice));

            auto quantity = static_cast<Quantity>(quantityDist(gen));

            activeOrders.push_back(ActiveOrder{nextOrderId, side});

            orderMsgs.push_back(OrderMessage{
                currTimestamp,
                nextOrderId++,
                price,
                quantity,
                EventType::New,
                side
            });
        }
    }

    return orderMsgs;
}

} // namespace engine
