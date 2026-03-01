#pragma once

#include <map>
#include <string>
#include <vector>

#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"

namespace engine {

class MatchingEngine {
public:
    std::vector<Trade> processOrder(const Order& order);

private:
    std::map<std::string, OrderBook> orderBooks;
};

} // namespace engine
