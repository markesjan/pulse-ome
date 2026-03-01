#pragma once

#include <vector>

#include "Order.hpp"

namespace engine {

class DataLoader {
public:
    static std::vector<Order> loadOrders(const std::string &filePath);
};

} // namespace engine
