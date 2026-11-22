/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  MatchingEngineWorkloadBenchmark.cpp
 * @brief Benchmarks for the MatchingEngine under realistic workload.
 */

#include <benchmark/benchmark.h>
#include "MatchingEngine.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_MatchingEngineFullWorkload(benchmark::State& state) {
    const int64_t n = state.range(0);

    struct Event {
        engine::EventType type;
        engine::OrderId id;
        engine::Price price;
        engine::Side side;
    };

    std::vector<Event> events;
    events.reserve(BATCH_SIZE);
    auto eventId = static_cast<engine::OrderId>(2 * n + 100);

    for (int64_t i = 0; i < BATCH_SIZE; i += 4) {
        engine::OrderId bidId = eventId++;
        engine::OrderId askId = eventId++;

        // 1. Insert Bid
        events.push_back({engine::EventType::New, bidId, 10000, engine::Side::Buy});
        // 2. Insert Ask
        events.push_back({engine::EventType::New, askId, 10010, engine::Side::Sell});
        // 3. Cancel Bid
        events.push_back({engine::EventType::FullCancel, bidId, 10000, engine::Side::Buy});
        // 4. Match Ask
        events.push_back({engine::EventType::New, eventId++, 10010, engine::Side::Buy});

        if (i % 8 == 0)
            std::swap(events[static_cast<std::size_t>(i + 2)],
                      events[static_cast<std::size_t>(i + 3)]);
    }

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>(
            static_cast<std::size_t>(2 * n + BATCH_SIZE + 1000), 12'000);
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n));

        for (int64_t i = 0; i < n / 2; ++i) {
            matchingEngine->processOrder({1'000'000'000, static_cast<engine::OrderId>(i), 9000, 10,
                                          engine::EventType::New, engine::Side::Buy},
                                         dummyTrades);
            matchingEngine->processOrder({1'000'000'000, static_cast<engine::OrderId>(n + i), 11000,
                                          10, engine::EventType::New, engine::Side::Sell},
                                         dummyTrades);
        }

        return matchingEngine;
    };

    auto matchingEngine = setupEngine();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (const auto& ev : events) {
            matchingEngine->processOrder({1'000'000'000, ev.id, ev.price, 10, ev.type, ev.side},
                                         trades);
        }

        state.PauseTiming();
        benchmark::DoNotOptimize(trades.data());
        matchingEngine = setupEngine();
        trades.clear();
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MatchingEngineFullWorkload)
    ->RangeMultiplier(4)
    ->Range(8, 1'000'000)
    ->Complexity(benchmark::oAuto)
    ->Unit(benchmark::kNanosecond);
