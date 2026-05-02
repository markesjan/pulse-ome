#include <gtest/gtest.h>

#include "MatchingEngine.hpp"

namespace engine {

class MatchingEngineTest : public testing::Test {
protected:
    MatchingEngine engine{10'000, 20'000};
    std::vector<Trade> trades;

    static Order makeOrder(OrderId id, Price p, Quantity q, Side s, EventType t = EventType::New) {
        return Order{1'000'000'000, id, p, q, t, s};
    }
};

TEST_F(MatchingEngineTest, ProcessOrderNewBidNoMatchAddsToBook) {
    engine.processOrder(makeOrder(1, 10000, 50, Side::Buy), trades);

    auto state = engine.bookState(1);
    ASSERT_EQ(state.size(), 1);
    EXPECT_EQ(state[0].bidPrice, 10000);
    EXPECT_EQ(state[0].bidQty, 50);
    EXPECT_EQ(state[0].askQty, 0);
    EXPECT_TRUE(trades.empty());
}

TEST_F(MatchingEngineTest, ProcessOrderNewAskNoMatchAddsToBook) {
    engine.processOrder(makeOrder(1, 15000, 100, Side::Sell), trades);

    auto state = engine.bookState(1);
    ASSERT_EQ(state.size(), 1);
    EXPECT_EQ(state[0].askPrice, 15000);
    EXPECT_EQ(state[0].askQty, 100);
    EXPECT_EQ(state[0].bidQty, 0);
    EXPECT_TRUE(trades.empty());
}

TEST_F(MatchingEngineTest, ProcessOrderBidExactMatchGeneratesTrade) {
    engine.processOrder(makeOrder(1, 10000, 50, Side::Sell), trades);
    engine.processOrder(makeOrder(2, 10000, 50, Side::Buy), trades);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].buyOrderId, 2);
    EXPECT_EQ(trades[0].sellOrderId, 1);
    EXPECT_EQ(trades[0].price, 10000);
    EXPECT_EQ(trades[0].quantity, 50);

    auto state = engine.bookState(1);
    EXPECT_TRUE(state.empty() || (state[0].askQty == 0 && state[0].bidQty == 0));
}

TEST_F(MatchingEngineTest, ProcessOrderAskExactMatchGeneratesTrade) {
    engine.processOrder(makeOrder(1, 10000, 50, Side::Buy), trades);
    engine.processOrder(makeOrder(2, 10000, 50, Side::Sell), trades);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].buyOrderId, 1);
    EXPECT_EQ(trades[0].sellOrderId, 2);
    EXPECT_EQ(trades[0].price, 10000);
    EXPECT_EQ(trades[0].quantity, 50);
}

TEST_F(MatchingEngineTest, ProcessOrderBidPartialMatchRestsRemainder) {
    engine.processOrder(makeOrder(1, 10000, 40, Side::Sell), trades);
    engine.processOrder(makeOrder(2, 10000, 100, Side::Buy), trades);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 40);

    auto state = engine.bookState(1);
    ASSERT_EQ(state.size(), 1);
    EXPECT_EQ(state[0].bidPrice, 10000);
    EXPECT_EQ(state[0].bidQty, 60);
    EXPECT_EQ(state[0].askQty, 0);
}

TEST_F(MatchingEngineTest, ProcessOrderAskPartialMatchRestsRemainder) {
    engine.processOrder(makeOrder(1, 10000, 40, Side::Buy), trades);
    engine.processOrder(makeOrder(2, 10000, 100, Side::Sell), trades);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 40);

    auto state = engine.bookState(1);
    ASSERT_EQ(state.size(), 1);
    EXPECT_EQ(state[0].askPrice, 10000);
    EXPECT_EQ(state[0].askQty, 60);
    EXPECT_EQ(state[0].bidQty, 0);
}

TEST_F(MatchingEngineTest, ProcessOrderBidConsumesMultipleAsks) {
    engine.processOrder(makeOrder(1, 10000, 50, Side::Sell), trades);
    engine.processOrder(makeOrder(2, 10100, 50, Side::Sell), trades);
    engine.processOrder(makeOrder(3, 10200, 50, Side::Sell), trades);

    engine.processOrder(makeOrder(4, 10200, 120, Side::Buy), trades);

    ASSERT_EQ(trades.size(), 3);

    EXPECT_EQ(trades[0].sellOrderId, 1);
    EXPECT_EQ(trades[0].price, 10000);
    EXPECT_EQ(trades[0].quantity, 50);

    EXPECT_EQ(trades[1].sellOrderId, 2);
    EXPECT_EQ(trades[1].price, 10100);
    EXPECT_EQ(trades[1].quantity, 50);

    EXPECT_EQ(trades[2].sellOrderId, 3);
    EXPECT_EQ(trades[2].price, 10200);
    EXPECT_EQ(trades[2].quantity, 20);

    auto state = engine.bookState(1);
    ASSERT_FALSE(state.empty());
    EXPECT_EQ(state[0].askPrice, 10200);
    EXPECT_EQ(state[0].askQty, 30);
}

TEST_F(MatchingEngineTest, ProcessOrderAskConsumesMultipleBids) {
    engine.processOrder(makeOrder(1, 10200, 50, Side::Buy), trades);
    engine.processOrder(makeOrder(2, 10100, 50, Side::Buy), trades);
    engine.processOrder(makeOrder(3, 10000, 50, Side::Buy), trades);

    engine.processOrder(makeOrder(4, 10000, 120, Side::Sell), trades);

    ASSERT_EQ(trades.size(), 3);

    EXPECT_EQ(trades[0].buyOrderId, 1);
    EXPECT_EQ(trades[0].price, 10200);
    EXPECT_EQ(trades[0].quantity, 50);

    EXPECT_EQ(trades[1].buyOrderId, 2);
    EXPECT_EQ(trades[1].price, 10100);
    EXPECT_EQ(trades[1].quantity, 50);

    EXPECT_EQ(trades[2].buyOrderId, 3);
    EXPECT_EQ(trades[2].price, 10000);
    EXPECT_EQ(trades[2].quantity, 20);

    auto state = engine.bookState(1);
    ASSERT_FALSE(state.empty());
    EXPECT_EQ(state[0].bidPrice, 10000);
    EXPECT_EQ(state[0].bidQty, 30);
}

TEST_F(MatchingEngineTest, ProcessOrderFullCancelRemovesOrderFromBook) {
    engine.processOrder(makeOrder(1, 10000, 100, Side::Buy), trades);
    engine.processOrder(makeOrder(1, 0, 100, Side::Buy, EventType::FullCancel), trades);

    auto state = engine.bookState(1);
    EXPECT_TRUE(state.empty() || state[0].bidQty == 0);
}

TEST_F(MatchingEngineTest, ProcessOrderPartialCancelReducesQuantityInBook) {
    engine.processOrder(makeOrder(1, 15000, 100, Side::Sell), trades);
    engine.processOrder(makeOrder(1, 0, 40, Side::Sell, EventType::PartialCancel), trades);

    auto state = engine.bookState(1);
    ASSERT_FALSE(state.empty());
    EXPECT_EQ(state[0].askPrice, 15000);
    EXPECT_EQ(state[0].askQty, 60);
}

TEST_F(MatchingEngineTest, GetBookStateReturnsCorrectDepth) {
    engine.processOrder(makeOrder(1, 10000, 10, Side::Buy), trades);
    engine.processOrder(makeOrder(2, 9900, 10, Side::Buy), trades);
    engine.processOrder(makeOrder(3, 10100, 10, Side::Sell), trades);
    engine.processOrder(makeOrder(4, 10200, 10, Side::Sell), trades);

    auto state = engine.bookState(2);
    ASSERT_EQ(state.size(), 2);

    EXPECT_EQ(state[0].bidPrice, 10000);
    EXPECT_EQ(state[0].askPrice, 10100);

    EXPECT_EQ(state[1].bidPrice, 9900);
    EXPECT_EQ(state[1].askPrice, 10200);
}

} // namespace engine
