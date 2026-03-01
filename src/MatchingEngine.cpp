#include "MatchingEngine.hpp"

namespace engine {

std::vector<Trade> MatchingEngine::processOrder(const Order& order) {
    OrderBook& book = this->orderBooks[order.symbol];
    return book.processOrder(order);
}

} // namespace engine
