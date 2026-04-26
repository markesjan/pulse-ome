#include <benchmark/benchmark.h>
#include "MatchingEngine.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_MatchingEngineMatchSingle(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n + BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<uint64_t>(i),
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

    auto setupEngine = [n] {
        const int64_t totalResting = n * SWEEP_BATCH_SIZE;
        auto matchingEngine = std::make_unique<engine::MatchingEngine>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(totalResting));

        for (int64_t i = 0; i < totalResting; ++i) {
            matchingEngine->processOrder({
                                             1'000'000'000,
                                             static_cast<uint64_t>(i),
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
                                             2'000'000,
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
     ->Complexity(benchmark::oAuto)
     ->Unit(benchmark::kNanosecond);
