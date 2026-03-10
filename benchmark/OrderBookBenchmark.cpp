#include <benchmark/benchmark.h>
#include "OrderBook.hpp"
#include "Order.hpp"

static void BM_InsertSamePrice(benchmark::State& state) {
    int64_t n = state.range(0);
    engine::OrderBook book;
    std::vector<engine::Trade> trades;

    for (int i = 0; i < n; ++i)
        book.processOrder({"ABCD", 1.0, static_cast<uint64_t>(i), 10000, 100, 1, 1},
                          trades);

    uint64_t nextId = n;
    for (auto _ : state) {
        book.processOrder({"ABCD", 1.0, nextId++, 10000, 100, 1, 1}, trades);
        benchmark::ClobberMemory();
    }

    state.SetComplexityN(n);
}

BENCHMARK(BM_InsertSamePrice)->RangeMultiplier(4)->Range(8, 1'000'000)
                             ->Complexity(benchmark::oAuto);

static void BM_InsertDiffPrices(benchmark::State& state) {
    int64_t n = state.range(0);
    engine::OrderBook book;
    std::vector<engine::Trade> trades;

    for (int i = 0; i < n; ++i)
        book.processOrder({"ABCD", 1.0, static_cast<uint64_t>(i),
                           10000 + static_cast<uint64_t>(i), 100, 1,
                           1}, trades);

    uint64_t nextId = n;
    for (auto _ : state) {
        uint64_t newPrice = 20000 + (nextId % n);
        book.processOrder({"ABCD", 1.0, nextId++, newPrice, 100, 1, 1}, trades);
        benchmark::ClobberMemory();
    }

    state.SetComplexityN(n);
}

BENCHMARK(BM_InsertDiffPrices)->RangeMultiplier(4)->Range(8, 1'000'000)
                              ->Complexity(benchmark::oAuto);

static void BM_LargeSweep(benchmark::State& state) {
    int64_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        engine::OrderBook book;
        std::vector<engine::Trade> trades;
        trades.reserve(n);

        for (int i = 0; i < n; ++i)
            book.processOrder({"ABCD", 1.0, static_cast<uint64_t>(i), 10000, 10, 1, -1},
                              trades);

        engine::Order sweepOrder{"ABCD", 1.0, static_cast<uint64_t>(n + 1), 10000,
                                 static_cast<uint32_t>(n * 10), 1, 1};
        state.ResumeTiming();

        book.processOrder(sweepOrder, trades);

        state.PauseTiming();
        benchmark::DoNotOptimize(trades.data());
    }

    state.SetComplexityN(n);
}

BENCHMARK(BM_LargeSweep)->RangeMultiplier(4)->Range(8, 4096)
                        ->Complexity(benchmark::oAuto);

static void BM_CancelWorstCase(benchmark::State& state) {
    int64_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        engine::OrderBook book;
        std::vector<engine::Trade> trades;

        for (int i = 0; i < n; ++i)
            book.processOrder({"ABCD", 1.0, static_cast<uint64_t>(i), 10000, 100, 1, 1},
                              trades);

        engine::Order cancelOrder{"ABCD", 1.0, static_cast<uint64_t>(n - 1), 10000, 100,
                                  3, 1};
        state.ResumeTiming();

        book.processOrder(cancelOrder, trades);

        state.PauseTiming();
        benchmark::ClobberMemory();
    }

    state.SetComplexityN(n);
}

BENCHMARK(BM_CancelWorstCase)->RangeMultiplier(4)->Range(8, 4096)
                             ->Complexity(benchmark::oAuto);

static void BM_MatchOneToOne(benchmark::State& state) {
    int64_t n = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        engine::OrderBook book;
        std::vector<engine::Trade> trades;
        trades.reserve(n);

        for (int i = 0; i < n; ++i)
            book.processOrder({"ABCD", 1.0, static_cast<uint64_t>(i), 10000, 10, 1, -1},
                              trades);

        state.ResumeTiming();

        uint64_t nextId = n;
        for (int i = 0; i < n; ++i)
            book.processOrder({"ABCD", 1.0, nextId++, 10000, 10, 1, 1}, trades);

        state.PauseTiming();
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(n);
}

BENCHMARK(BM_MatchOneToOne)->RangeMultiplier(4)->Range(8, 1'000'000)
                           ->Complexity(benchmark::oAuto);

static void BM_MixedWorkload(benchmark::State& state) {
    int64_t n = state.range(0);

    struct Event {
        uint8_t type;
        uint64_t price;
        int8_t side;
    };
    std::vector<Event> events;
    events.reserve(n);
    for (int i = 0; i < n; ++i) {
        if (i % 4 == 0)
            events.push_back({1, 10000, 1}); // Insert Buy
        else if (i % 4 == 1)
            events.push_back({1, 10010, -1}); // Insert Sell
        else if (i % 4 == 2)
            events.push_back({2, 10000, 1}); // Cancel Buy
        else
            events.push_back({1, 10010, 1}); // Match (Buy hits Sell)
    }

    for (auto _ : state) {
        state.PauseTiming();
        engine::OrderBook book;
        std::vector<engine::Trade> trades;
        trades.reserve(n);
        state.ResumeTiming();

        uint64_t id = 1;
        for (const auto& ev : events) {
            uint64_t targetId = ev.type == 2 ? (id - 2) : id;
            book.processOrder({"ABCD", 1.0, targetId, ev.price, 10, ev.type, ev.side},
                              trades);
            if (ev.type == 1)
                id++;
        }

        benchmark::ClobberMemory();
    }
    state.SetComplexityN(n);
}

BENCHMARK(BM_MixedWorkload)->RangeMultiplier(4)->Range(8, 65'536)
                           ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
