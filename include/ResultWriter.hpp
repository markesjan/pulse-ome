#pragma once

#include <vector>

#include "Trade.hpp"

namespace engine {

class ResultWriter {
public:
    static void writeTrades(const std::vector<Trade>& trades, const std::string& filePath);
};

} // namespace engine
