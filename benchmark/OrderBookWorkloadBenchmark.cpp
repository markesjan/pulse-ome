#include <benchmark/benchmark.h>
#include "OrderBook.hpp"

constexpr int64_t BATCH_SIZE = 10'000;

static void BM_OrderBookFullWorkload(benchmark::State& state) {
    const int64_t n = state.range(0);

    struct Event {
        engine::EventType type;
        engine::OrderId id;
        engine::Price price;
        engine::Side side;
    };

    std::vector<Event> events;
    events.reserve(BATCH_SIZE);
    auto eventId = static_cast<engine::OrderId>(n + 1'000'000);

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
            std::swap(events[static_cast<size_t>(i + 2)], events[static_cast<size_t>(i + 3)]);
    }

    auto setupBook = [n] {
        auto book = std::make_unique<engine::OrderBook>();
        std::vector<engine::Trade> dummyTrades;
        dummyTrades.reserve(static_cast<size_t>(n));

        for (int64_t i = 0; i < n / 2; ++i) {
            book->processOrder({1.0, static_cast<uint64_t>(i), 9000, 10,
                                engine::EventType::New, engine::Side::Buy}, dummyTrades);
            book->processOrder({1.0, static_cast<uint64_t>(n + i), 11000, 10,
                                engine::EventType::New, engine::Side::Sell}, dummyTrades);
        }

        return book;
    };

    auto book = setupBook();
    std::vector<engine::Trade> trades;
    trades.reserve(BATCH_SIZE);

    while (state.KeepRunningBatch(BATCH_SIZE)) {
        for (const auto& ev : events) {
            book->processOrder({
                                   1.0,
                                   ev.id,
                                   ev.price,
                                   10,
                                   ev.type,
                                   ev.side
                               }, trades);
        }

        state.PauseTiming();
        benchmark::DoNotOptimize(trades.data());
        book = setupBook();
        trades.clear();
        state.ResumeTiming();
    }

    state.SetComplexityN(n);
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_OrderBookFullWorkload)
    ->RangeMultiplier(4)
    ->Range(8, 100'000)
    ->Complexity(benchmark::oAuto)
    ->Unit(benchmark::kNanosecond);
