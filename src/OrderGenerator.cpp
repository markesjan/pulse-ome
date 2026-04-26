#include "OrderGenerator.hpp"

#include <random>

namespace engine {

std::vector<Order> OrderGenerator::generate(uint32_t seed,
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
        uint64_t id;
        Side side;
    };

    std::vector<Order> orders;
    orders.reserve(ordersCount);
    std::vector<ActiveOrder> activeOrders;
    activeOrders.reserve(ordersCount);

    uint64_t nextOrderId = 0;
    double currTimestamp = 1.0;
    std::string symbol("ABCD");

    for (std::size_t i = 0; i < ordersCount; ++i) {
        currTimestamp += 0.001;

        if (eventDist(gen) <= static_cast<int>(cancelPercent) && !activeOrders.empty()) {
            std::size_t window_size = std::min<std::size_t>(activeOrders.size(), 100);
            std::uniform_int_distribution recentDist(activeOrders.size() - window_size, activeOrders.size() - 1);
            std::size_t cancelIdx = recentDist(gen);
            ActiveOrder orderToCancel = activeOrders[cancelIdx];

            std::uniform_int_distribution typeDist(2, 3);
            auto rawCancelType = typeDist(gen);
            auto cancelType = rawCancelType == 3 ? EventType::FullCancel : EventType::PartialCancel;

            uint32_t cancelQty = 0;

            if (cancelType == EventType::FullCancel) {
                cancelQty = 0;
                activeOrders[cancelIdx] = activeOrders.back();
                activeOrders.pop_back();
            } else {
                std::uniform_int_distribution<uint32_t> partialQtyDist(1, 10);
                cancelQty = partialQtyDist(gen);
            }

            orders.push_back(Order{
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
            auto price = static_cast<uint64_t>(std::max(1.0, rawPrice));

            auto quantity = static_cast<uint32_t>(quantityDist(gen));

            activeOrders.push_back(ActiveOrder{nextOrderId, side});

            orders.push_back(Order{
                currTimestamp,
                nextOrderId++,
                price,
                quantity,
                EventType::New,
                side
            });
        }
    }

    return orders;
}

} // namespace engine
