#include "OrderGenerator.hpp"

#include <random>

namespace engine {

std::vector<Order> OrderGenerator::generate(uint32_t seed,
                                            size_t ordersCnt,
                                            uint64_t basePrice,
                                            size_t cancelPercentage) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution eventDist(1, 100);
    std::uniform_int_distribution sideDist(0, 1);
    std::uniform_int_distribution quantityDist(1, 100);
    std::normal_distribution<> priceDist(basePrice, 15.0);

    struct ActiveOrder {
        uint64_t id;
        int8_t side;
    };

    std::vector<Order> orders;
    orders.reserve(ordersCnt);
    std::vector<ActiveOrder> activeOrders;
    activeOrders.reserve(ordersCnt);

    uint64_t nextOrderId = 0;
    double currTimestamp = 1.0;

    for (size_t i = 0; i < ordersCnt; ++i) {
        currTimestamp += 0.001;

        if (eventDist(gen) <= cancelPercentage && !activeOrders.empty()) {
            std::uniform_int_distribution<size_t> idxDist(0, activeOrders.size() - 1);
            size_t cancelIdx = idxDist(gen);
            ActiveOrder orderToCancel = activeOrders[cancelIdx];

            std::uniform_int_distribution typeDist(2, 3);
            auto cancelType = static_cast<uint8_t>(typeDist(gen));

            uint32_t cancelQty = 0;

            if (cancelType == 3) {
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
            auto side = sideDist(gen) == 0 ? static_cast<int8_t>(1) : static_cast<int8_t>(-1);
            auto price = static_cast<uint64_t>(std::max(1.0, priceDist(gen)));
            auto quantity = static_cast<uint32_t>(quantityDist(gen));

            activeOrders.push_back(ActiveOrder{nextOrderId, side});

            orders.push_back(Order{
                currTimestamp,
                nextOrderId++,
                price,
                quantity,
                static_cast<uint8_t>(1),
                side
            });
        }
    }

    return orders;
}

} // namespace engine
