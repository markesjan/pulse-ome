/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Application.hpp
 * @brief Definition of the Application class and program execution logic.
 */

#pragma once

#include <optional>

#include "Config.hpp"
#include "MatchingEngine.hpp"
#include "Order.hpp"
#include "OrderIdMapper.hpp"
#include "Trade.hpp"

namespace engine {

class Application {
public:
    explicit Application(const Config& cfg);
    void run();

private:
    Config config;
    std::optional<OrderIdMapper> idMapper;
    std::optional<MatchingEngine> matchingEngine;

    std::vector<OrderMessage> orderMsgs;
    std::vector<Trade> trades;
    std::vector<uint64_t> latencies;

    void prepareOrders();
    void applyIsolation() const;
    double matchOrders();
    static double countCyclesPerNs();
};

} // namespace engine
