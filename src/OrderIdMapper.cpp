#include "OrderIdMapper.hpp"

#include <stdexcept>

namespace engine {

OrderIdMapper::OrderIdMapper(std::size_t maxOrders) : nextInternalId(0) {
    externalToInternalId.reserve(maxOrders);
    internalToExternalId.reserve(maxOrders);
}

OrderId OrderIdMapper::toInternal(OrderId externalId) {
    auto it = externalToInternalId.find(externalId);
    if (it != externalToInternalId.end())
        return it->second;

    OrderId newInternalId = ++nextInternalId;
    externalToInternalId.emplace(externalId, newInternalId);

    if (newInternalId >= internalToExternalId.size())
        internalToExternalId.push_back(externalId);

    return newInternalId;
}

OrderId OrderIdMapper::toExternal(OrderId internalId) const {
    if (internalId < internalToExternalId.size())
        return internalToExternalId[internalId];

    throw std::out_of_range("Unknown internal order ID");
}

} // namespace engine
