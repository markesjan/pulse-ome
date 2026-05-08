/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  MatchingEngineInsertBenchmark.cpp
 * @brief Benchmarks focused on the order insertion performance of the MatchingEngine.
 */

#include <benchmark/benchmark.h>
#include "MatchingEngine.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_MatchingEngineInsertSamePrice(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>(
            static_cast<std::size_t>(n + BATCH_SIZE + 1000), 20'000);
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(i),
                                             10000,
                                             100,
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, dummyTrades);
        }
        return matchingEngine;
    };

    auto matchingEngine = setupEngine();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);
    int64_t nextId = n;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(nextId++),
                                             10000,
                                             100,
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, trades);
        }

        state.PauseTiming();
        matchingEngine = setupEngine();
        trades.clear();
        nextId = n;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MatchingEngineInsertSamePrice)
    ->RangeMultiplier(4)
    ->Range(8, 1'000'000)
    ->Complexity(benchmark::oAuto)
    ->Unit(benchmark::kNanosecond);

static void BM_MatchingEngineInsertNewPrice(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>(
            static_cast<std::size_t>(n + BATCH_SIZE + 1000), 2'500'000);
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(i),
                                             static_cast<engine::Price>(10000 + i),
                                             100,
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, dummyTrades);
        }

        return matchingEngine;
    };

    auto matchingEngine = setupEngine();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);
    int64_t nextId = n;
    engine::Price nextPrice = 2'000'000;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(nextId++),
                                             nextPrice++,
                                             100,
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, trades);
        }

        state.PauseTiming();
        matchingEngine = setupEngine();
        trades.clear();
        nextId = n;
        nextPrice = 2'000'000;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MatchingEngineInsertNewPrice)
    ->RangeMultiplier(4)
    ->Range(8, 1'000'000)
    ->Complexity(benchmark::oAuto)
    ->Unit(benchmark::kNanosecond);
