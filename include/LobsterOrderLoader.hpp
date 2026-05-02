#pragma once

#include <vector>
#include <string>

#include "Order.hpp"
#include "OrderIdMapper.hpp"

namespace engine {

class LobsterOrderLoader {
public:
    static std::vector<Order> load(const std::string& filePath, OrderIdMapper& idMapper);
};

} // namespace engine
