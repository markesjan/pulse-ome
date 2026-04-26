#pragma once

#include <vector>
#include <string>

#include "Order.hpp"

namespace engine {

class LobsterOrderLoader {
public:
    static std::vector<Order> load(const std::string& filePath);
};

} // namespace engine
