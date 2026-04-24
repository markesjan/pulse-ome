#include <benchmark/benchmark.h>
#include "OrderBook.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_OrderBookCancelPartial(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupBook = [n] {
        auto book = std::make_unique<engine::OrderBook>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n; ++i) {
            book->processOrder({
                                   1.0,
                                   static_cast<engine::OrderId>(i),
                                   10'000,
                                   100,
                                   engine::EventType::New,
                                   engine::Side::Buy
                               }, dummyTrades);
        }

        return book;
    };

    auto book = setupBook();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);
    int64_t targetId = n;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < n; ++i) {
            book->processOrder({
                                   1.0,
                                   static_cast<engine::Price>(targetId++),
                                   10'000,
                                   10,
                                   engine::EventType::PartialCancel,
                                   engine::Side::Buy
                               }, trades);
        }

        state.PauseTiming();
        book = setupBook();
        trades.clear();
        targetId = n;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

// BENCHMARK(BM_OrderBookCancelPartial)
//     ->RangeMultiplier(2)
//     ->Range(8, 100'000)
//     ->Complexity(benchmark::oAuto)
//     ->Unit(benchmark::kNanosecond);

static void BM_OrderBookCancelFull(benchmark::State& state) {
    const int64_t n = state.range(0);

    auto setupBook = [n] {
        auto book = std::make_unique<engine::OrderBook>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<size_t>(n + BATCH_SIZE));

        for (int64_t i = 0; i < n; ++i) {
            book->processOrder({
                                   1.0,
                                   static_cast<engine::OrderId>(i),
                                   10'000,
                                   100,
                                   engine::EventType::New,
                                   engine::Side::Buy
                               }, dummyTrades);
        }

        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            book->processOrder({
                                   1.0,
                                   static_cast<engine::OrderId>(n + i),
                                   10'000,
                                   100,
                                   engine::EventType::New,
                                   engine::Side::Buy
                               }, dummyTrades);
        }

        return book;
    };

    auto book = setupBook();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);
    int64_t cancelId = n;

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (int64_t i = 0; i < BATCH_SIZE; ++i) {
            book->processOrder({
                                   1.0,
                                   static_cast<engine::OrderId>(cancelId++),
                                   10'000,
                                   100,
                                   engine::EventType::FullCancel,
                                   engine::Side::Buy
                               }, trades);
        }

        state.PauseTiming();
        book = setupBook();
        trades.clear();
        cancelId = n;
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

// BENCHMARK(BM_OrderBookCancelFull)
//     ->RangeMultiplier(4)
//     ->Range(8, 1'000'000)
//     ->Complexity(benchmark::oAuto)
//     ->Unit(benchmark::kNanosecond);
