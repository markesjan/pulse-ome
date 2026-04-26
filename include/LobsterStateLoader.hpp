#pragma once

#include <string>
#include <vector>

#include "OrderBookLevel.hpp"

namespace engine {

using LobStates = std::vector<std::vector<OrderBookLevel>>;

class LobsterStateLoader {
public:
    static LobStates load(const std::string& filePath, std::size_t depth);
};

} // namespace engine
