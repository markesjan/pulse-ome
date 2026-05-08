/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  LobsterStateLoader.cpp
 * @brief Implementation of the LobsterStateLoader class.
 */

#include "LobsterStateLoader.hpp"

#include <cstring>
#include <fstream>
#include <print>

namespace engine {

LobStates LobsterStateLoader::load(const std::string& filePath, std::size_t depth) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::println(stderr, "Couldn't open file: {}", filePath);
        exit(1);
    }

    std::vector<std::vector<OrderBookLevel>> states;
    states.reserve(1'000'000);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        const char* p = line.c_str();
        char* end;

        std::vector<OrderBookLevel> currLevels;
        currLevels.reserve(depth);

        for (std::size_t i = 0; i < depth; ++i) {
            if (*p == '\0')
                break;

            Price askPrice = std::strtoull(p, &end, 10);
            if (p == end)
                break;
            p = end + (*end == ',' ? 1 : 0);

            Quantity askQty = static_cast<Quantity>(std::strtoul(p, &end, 10));
            p = end + (*end == ',' ? 1 : 0);

            Price bidPrice = std::strtoull(p, &end, 10);
            p = end + (*end == ',' ? 1 : 0);

            Quantity bidQty = static_cast<Quantity>(std::strtoul(p, &end, 10));
            p = end + (*end == ',' ? 1 : 0);

            currLevels.emplace_back(OrderBookLevel{askPrice, askQty, bidPrice, bidQty});
        }

        states.push_back(std::move(currLevels));
    }

    states.shrink_to_fit();
    std::println("Loaded {} order book states.", states.size());

    return states;
}

} // namespace engine
