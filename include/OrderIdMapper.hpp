#pragma once

#include <unordered_map>
#include <vector>

#include "Types.hpp"

namespace engine {

class OrderIdMapper {
public:
    explicit OrderIdMapper(std::size_t maxOrders);

    OrderId toInternal(OrderId externalId);
    OrderId toExternal(OrderId internalId) const;

private:
    OrderId nextInternalId;
    std::unordered_map<OrderId, OrderId> externalToInternalId;
    std::vector<OrderId> internalToExternalId;
};

} // namespace engine
