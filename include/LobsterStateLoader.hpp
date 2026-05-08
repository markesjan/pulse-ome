/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  LobsterStateLoader.hpp
 * @brief Declaration of the LobsterStateLoader class for loading LOBSTER order book snapshots.
 */

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
