#include <benchmark/benchmark.h>
#include "MatchingEngine.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_MatchingEngineCancelPartial(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n; ++i) {
            matchingEngine->processOrder({
                                             1.0,
                                             static_cast<engine::OrderId>(i),
                                             10'000,
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
    int64_t targetId = n;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1.0,
                                             static_cast<engine::Price>(targetId++),
                                             10'000,
                                             10,
                                             engine::EventType::PartialCancel,
                                             engine::Side::Buy
                                         }, trades);
        }

        state.PauseTiming();
        matchingEngine = setupEngine();
        trades.clear();
        targetId = n;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MatchingEngineCancelPartial)
    ->RangeMultiplier(4)
    ->Range(8, 1'000'000)
    ->Complexity(benchmark::oAuto)
    ->Unit(benchmark::kNanosecond);

static void BM_MatchingEngineCancelFull(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupEngine = [n] {
        auto matchingEngine = std::make_unique<engine::MatchingEngine>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<std::size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n; ++i) {
            matchingEngine->processOrder({
                                             1.0,
                                             static_cast<engine::OrderId>(i),
                                             10'000,
                                             100,
                                             engine::EventType::New,
                                             engine::Side::Buy
                                         }, dummyTrades);
        }

        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1.0,
                                             static_cast<engine::OrderId>(n + i),
                                             10'000,
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
    int64_t cancelId = n;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            matchingEngine->processOrder({
                                             1.0,
                                             static_cast<engine::OrderId>(cancelId++),
                                             10'000,
                                             100,
                                             engine::EventType::FullCancel,
                                             engine::Side::Buy
                                         }, trades);
        }

        state.PauseTiming();
        matchingEngine = setupEngine();
        trades.clear();
        cancelId = n;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_MatchingEngineCancelFull)
    ->RangeMultiplier(4)
    ->Range(8, 1'000'000)
    ->Complexity(benchmark::oAuto)
    ->Unit(benchmark::kNanosecond);
