#include <gtest/gtest.h>
#include <print>

#include "MatchingEngine.hpp"
#include "OrderGenerator.hpp"

namespace engine {

class SyntheticStressTest : public testing::Test {
protected:
    MatchingEngine matchingEngine;
    std::vector<Trade> trades;

    void SetUp() override {
        trades.reserve(100'000);
    }
};

TEST_F(SyntheticStressTest, DefaultGeneratorSetup) {
    constexpr uint32_t SEED = 42;
    constexpr std::size_t ORDER_COUNT = 100'000;

    auto orders = OrderGenerator::generate(SEED, ORDER_COUNT);
    ASSERT_EQ(orders.size(), ORDER_COUNT) << "Generated unexpected amount of orders";

    for (std::size_t i = 0; i < orders.size(); ++i) {
        matchingEngine.processOrder(orders[i], trades);

        auto bookState = matchingEngine.getBookState(1);
        if (bookState.empty())
            continue;

        const auto& top = bookState[0];

        ASSERT_GE(top.askQty, 0) << "Ask quantity underflow (index: " << i << ")";
        ASSERT_GE(top.bidQty, 0) << "Bid quantity underflow (index: " << i << ")";

        if (top.askQty > 0 && top.bidQty > 0)
            ASSERT_GT(top.askPrice, top.bidPrice) << "Unexecuted trade (index: " << i << ")";
    }

    ASSERT_GT(trades.size(), 0) << "Zero trades were executed";
}

TEST_F(SyntheticStressTest, ZeroCancels) {
    constexpr uint32_t SEED = 42;
    constexpr std::size_t ORDER_COUNT = 100'000;
    constexpr std::size_t CANCEL_PERCENT = 0;

    auto orders = OrderGenerator::generate(SEED, ORDER_COUNT, CANCEL_PERCENT);
    ASSERT_EQ(orders.size(), ORDER_COUNT) << "Generated unexpected amount of orders";

    for (std::size_t i = 0; i < orders.size(); ++i) {
        matchingEngine.processOrder(orders[i], trades);

        auto bookState = matchingEngine.getBookState(1);
        if (bookState.empty())
            continue;

        const auto& top = bookState[0];

        ASSERT_GE(top.askQty, 0) << "Ask quantity underflow (index: " << i << ")";
        ASSERT_GE(top.bidQty, 0) << "Bid quantity underflow (index: " << i << ")";

        if (top.askQty > 0 && top.bidQty > 0)
            ASSERT_GT(top.askPrice, top.bidPrice) << "Unexecuted trade (index: " << i << ")";
    }

    ASSERT_GT(trades.size(), 0) << "Zero trades were executed";
}

TEST_F(SyntheticStressTest, HighCancels) {
    constexpr uint32_t SEED = 42;
    constexpr std::size_t ORDER_COUNT = 100'000;
    constexpr std::size_t CANCEL_PERCENT = 80;

    auto orders = OrderGenerator::generate(SEED, ORDER_COUNT, CANCEL_PERCENT);
    ASSERT_EQ(orders.size(), ORDER_COUNT) << "Generated unexpected amount of orders";

    for (std::size_t i = 0; i < orders.size(); ++i) {
        matchingEngine.processOrder(orders[i], trades);

        auto bookState = matchingEngine.getBookState(1);
        if (bookState.empty())
            continue;

        const auto& top = bookState[0];

        ASSERT_GE(top.askQty, 0) << "Ask quantity underflow (index: " << i << ")";
        ASSERT_GE(top.bidQty, 0) << "Bid quantity underflow (index: " << i << ")";

        if (top.askQty > 0 && top.bidQty > 0)
            ASSERT_GT(top.askPrice, top.bidPrice) << "Unexecuted trade (index: " << i << ")";
    }

    ASSERT_GT(trades.size(), 0) << "Zero trades were executed";
}

} // namespace engine
