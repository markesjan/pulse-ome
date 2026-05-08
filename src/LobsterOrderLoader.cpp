/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  LobsterOrderLoader.cpp
 * @brief Implementation of the LobsterOrderLoader class.
 */

#include "LobsterOrderLoader.hpp"

#include <cstring>
#include <fstream>
#include <print>

namespace engine {

std::vector<OrderMessage> LobsterOrderLoader::load(const std::string& filePath,
                                                   OrderIdMapper& idMapper) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::println(stderr, "Couldn't open file: {}", filePath);
        exit(1);
    }

    std::vector<OrderMessage> orderMsgs;
    orderMsgs.reserve(1'000'000);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        const char* p = line.c_str();
        char* end;

        if (!std::strchr(p, ','))
            continue;

        double rawTimestamp = std::strtod(p, &end);
        auto fixedTimestamp = static_cast<Timestamp>(rawTimestamp * 1'000'000'000);
        if (p == end)
            continue;
        p = end + (*end == ',' ? 1 : 0);

        auto rawEventType = static_cast<uint8_t>(std::strtol(p, &end, 10));
        if (p == end)
            continue;
        p = end + (*end == ',' ? 1 : 0);

        if (rawEventType >= 1 && rawEventType <= 3) {
            auto rawExternalId = static_cast<OrderId>(std::strtoull(p, &end, 10));
            if (p == end)
                continue;
            p = end + (*end == ',' ? 1 : 0);

            auto quantity = static_cast<Quantity>(std::strtoul(p, &end, 10));
            if (p == end)
                continue;
            p = end + (*end == ',' ? 1 : 0);

            auto price = static_cast<Price>(std::strtoull(p, &end, 10));
            if (p == end)
                continue;
            p = end + (*end == ',' ? 1 : 0);

            auto rawSide = static_cast<int8_t>(std::strtol(p, &end, 10));

            orderMsgs.emplace_back(OrderMessage{
                fixedTimestamp,
                idMapper.toInternal(rawExternalId),
                price,
                quantity,
                static_cast<EventType>(rawEventType),
                static_cast<Side>(rawSide),
            });
        }
    }

    orderMsgs.shrink_to_fit();
    std::println("Loaded {} relevant order events.", orderMsgs.size());

    return orderMsgs;
}

} // namespace engine
