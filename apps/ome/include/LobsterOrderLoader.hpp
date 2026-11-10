/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  LobsterOrderLoader.hpp
 * @brief Definition of the LobsterOrderLoader class for loading LOBSTER message data.
 */

#pragma once

#include <string>
#include <vector>

#include "Order.hpp"
#include "OrderIdMapper.hpp"

namespace engine {

class LobsterOrderLoader {
public:
    static std::vector<OrderMessage> load(const std::string& filePath, OrderIdMapper& idMapper);
};

} // namespace engine
