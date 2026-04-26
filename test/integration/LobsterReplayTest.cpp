#include <gtest/gtest.h>
#include <print>

#include "MatchingEngine.hpp"
#include "LobsterOrderLoader.hpp"
#include "LobsterStateLoader.hpp"

namespace engine {

struct LobsterScenario {
    std::string symbol;
    std::size_t depth;
    std::string messageFile;
    std::string orderbookFile;

    friend std::ostream& operator<<(std::ostream& os, const LobsterScenario& s) {
        return os << s.symbol << "_lvl" << s.depth;
    }
};

class LobsterReplayTest : public testing::TestWithParam<LobsterScenario> {
protected:
    MatchingEngine matchingEngine;
    std::vector<Trade> trades;

    void SetUp() override {
        trades.reserve(10'000);
    }
};

TEST_P(LobsterReplayTest, ReplayAndVerifyState) {
    const auto& [symbol, depth, msgPath, bookPath] = GetParam();

    auto orders = LobsterOrderLoader::load(msgPath);
    auto expectedStates = LobsterStateLoader::load(bookPath, depth);

    ASSERT_FALSE(orders.empty()) << "Empty data for " << symbol;
    ASSERT_EQ(orders.size(), expectedStates.size()) << "Row count mismatch for " << symbol;

    if (!expectedStates.empty()) {
        const auto& firstState = expectedStates[0];
        for (std::size_t lvl = 0; lvl < depth; ++lvl) {
            if (firstState[lvl].askQty > 0) {
                matchingEngine.processOrder(Order{0, 999999900 + lvl, firstState[lvl].askPrice,
                                                  firstState[lvl].askQty, EventType::New,
                                                  Side::Sell},
                                            trades);
            }
            if (firstState[lvl].bidQty > 0) {
                matchingEngine.processOrder(Order{0, 999999800 + lvl, firstState[lvl].bidPrice,
                                                  firstState[lvl].bidQty, EventType::New,
                                                  Side::Buy},
                                            trades);
            }
        }
    }

    for (std::size_t i = 0; i < orders.size(); ++i) {
        matchingEngine.processOrder(orders[i], trades);

        auto actualState = matchingEngine.getBookState(depth);
        const auto& expectedState = expectedStates[i];

        ASSERT_EQ(actualState.size(), expectedState.size())
            << "Order book level mismatch at row " << i << " for " << symbol;

        for (std::size_t lvl = 0; lvl < depth; ++lvl) {
            EXPECT_EQ(actualState[lvl].askPrice, expectedState[lvl].askPrice)
                << "[" << symbol << " | Row " << i << " | Lvl " << lvl << "] Ask Price mismatch!";
            EXPECT_EQ(actualState[lvl].askQty, expectedState[lvl].askQty)
                << "[" << symbol << " | Row " << i << " | Lvl " << lvl << "] Ask Qty mismatch!";
            EXPECT_EQ(actualState[lvl].bidPrice, expectedState[lvl].bidPrice)
                << "[" << symbol << " | Row " << i << " | Lvl " << lvl << "] Bid Price mismatch!";
            EXPECT_EQ(actualState[lvl].bidQty, expectedState[lvl].bidQty)
                << "[" << symbol << " | Row " << i << " | Lvl " << lvl << "] Bid Qty mismatch!";
        }

        if (HasFailure()) {
            std::println(stderr, "Market desynchronized ({})", symbol);
            std::println(stderr, "Message ID: {}, Type: {}", orders[i].orderId,
                         static_cast<int>(orders[i].eventType));
            FAIL() << "Stopping test for " << symbol;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    StaticLobsterFiles,
    LobsterReplayTest,
    ::testing::Values(
        // LobsterScenario{"MSFT", 1,
        // "../data/LOBSTER/MSFT/MSFT_2012-06-21_message_lvl1.csv",
        // "../data/LOBSTER/MSFT/MSFT_2012-06-21_orderbook_lvl1.csv"},
        // LobsterScenario{"MSFT", 5,
        // "../data/LOBSTER/MSFT/MSFT_2012-06-21_message_lvl5.csv",
        // "../data/LOBSTER/MSFT/MSFT_2012-06-21_orderbook_lvl5.csv"},
        // LobsterScenario{"MSFT", 10,
        // "../data/LOBSTER/MSFT/MSFT_2012-06-21_message_lvl10.csv",
        // "../data/LOBSTER/MSFT/MSFT_2012-06-21_orderbook_lvl10.csv"}
    ));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(LobsterReplayTest);

} // namespace engine
