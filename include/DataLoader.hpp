#pragma once

#include <vector>
#include <string>

#include "Order.hpp"

namespace engine {

class DataLoader {
public:
    static std::vector<Order> loadOrders(const std::string &filePath);
};

} // namespace engine
