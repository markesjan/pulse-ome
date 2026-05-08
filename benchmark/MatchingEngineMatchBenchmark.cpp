/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  MatchingEngineMatchBenchmark.cpp
 * @brief Benchmarks focused on the matching performance of the MatchingEngine.
 */

#include <benchmark/benchmark.h>
#include "MatchingEngine.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_MatchingEngineMatchSingle(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>(
            static_cast<std::size_t>(n + 2 * BATCH_SIZE + 1000), 20'000);
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n + BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(i),
                                             10000,
                                             10,
                                             engine::EventType::New,
                                             engine::Side::Sell
                                         }, dummyTrades);
        }

        return matchingEngine;
    };

    auto matchingEngine = setupEngine();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);
    int64_t nextId = n + BATCH_SIZE;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(nextId++),
                                             10000,
                                             10,
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, trades);
        }

        state.PauseTiming();
        benchmark::DoNotOptimize(trades.data());
        matchingEngine = setupEngine();
        trades.clear();
        nextId = n + BATCH_SIZE;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MatchingEngineMatchSingle)
     ->RangeMultiplier(4)
     ->Range(8, 1'000'000)
     ->Complexity(benchmark::oAuto)
     ->Unit(benchmark::kNanosecond);

constexpr int64_t SWEEP_BATCH_SIZE = 100;

static void BM_MatchingEngineMatchSweep(benchmark::State& state) {
    const int64_t n = state.range(0);

    const int64_t totalResting = n * SWEEP_BATCH_SIZE;
    const engine::Price maxGeneratedPrice = static_cast<engine::Price>(10000 + totalResting);
    const engine::Price sweepPriceLimit = maxGeneratedPrice + 100;

    auto setupEngine = [totalResting, maxGeneratedPrice] {
        std::size_t maxOrders = static_cast<std::size_t>(totalResting + SWEEP_BATCH_SIZE + 1000);
        engine::Price maxPrice = maxGeneratedPrice + 500;

        auto matchingEngine = std::make_unique<engine::MatchingEngine>(maxOrders, maxPrice);
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(totalResting));

        for (int64_t i = 0; i < totalResting; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(i),
                                             static_cast<engine::Price>(10000 + i),
                                             10,
                                             engine::EventType::New,
                                             engine::Side::Sell
                                         }, dummyTrades);
        }

        return matchingEngine;
    };

    auto matchingEngine = setupEngine();
    std::vector<engine::Trade> trades;
    trades.reserve(static_cast<std::size_t>(n * SWEEP_BATCH_SIZE));
    int64_t nextId = n * SWEEP_BATCH_SIZE;

    while (state.KeepRunningBatch(SWEEP_BATCH_SIZE)) {
        for (int64_t i = 0; i < SWEEP_BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<engine::OrderId>(nextId++),
                                             sweepPriceLimit,
                                             static_cast<engine::Quantity>(n * 10),
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, trades);
        }

        state.PauseTiming();
        benchmark::DoNotOptimize(trades.data());
        matchingEngine = setupEngine();
        trades.clear();
        nextId = n * SWEEP_BATCH_SIZE;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations() * n);
}

BENCHMARK(BM_MatchingEngineMatchSweep)
     ->RangeMultiplier(2)
     ->Range(8, 50'000)
     ->Complexity(benchmark::oN)
     ->Unit(benchmark::kNanosecond);
