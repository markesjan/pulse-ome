#include <gtest/gtest.h>
#include "MatchingEngine.hpp"

namespace engine {

class PriceTimePriorityTest : public testing::Test {
protected:
    MatchingEngine matchingEngine;
    std::vector<Trade> trades;

    void verifyTop(Price askPrice, Quantity askQty, Price bidPrice, Quantity bidQty) const {
        auto state = matchingEngine.getBookState(1);
        ASSERT_FALSE(state.empty());
        EXPECT_EQ(state[0].askPrice, askPrice);
        EXPECT_EQ(state[0].askQty, askQty);
        EXPECT_EQ(state[0].bidPrice, bidPrice);
        EXPECT_EQ(state[0].bidQty, bidQty);
    }
};

TEST_F(PriceTimePriorityTest, BasicMatchingAndCancels) {
    matchingEngine.processOrder({1.0, 101, 10000, 100, EventType::New, Side::Buy}, trades);
    matchingEngine.processOrder({1.1, 102, 10000, 50, EventType::New, Side::Sell}, trades);
    matchingEngine.processOrder({1.2, 103, 10100, 100, EventType::New, Side::Sell}, trades);
    matchingEngine.processOrder({1.3, 104, 10200, 100, EventType::New, Side::Sell}, trades);
    matchingEngine.processOrder({1.4, 105, 10200, 150, EventType::New, Side::Buy}, trades);
    verifyTop(10200, 50, 10000, 50);

    // Modify resting liquidity
    matchingEngine.processOrder({1.5, 104, 10200, 20, EventType::PartialCancel, Side::Sell},
                                trades);
    verifyTop(10200, 30, 10000, 50);
    matchingEngine.processOrder({1.6, 101, 10000, 50, EventType::FullCancel, Side::Buy}, trades);
    verifyTop(10200, 30, 0, 0);

    // Match aggressive bid
    matchingEngine.processOrder({1.7, 106, 10200, 50, EventType::New, Side::Buy}, trades);
    verifyTop(0, 0, 10200, 20);

    ASSERT_EQ(trades.size(), 4);

    EXPECT_EQ(trades[0].buyOrderId, 101);
    EXPECT_EQ(trades[0].sellOrderId, 102);
    EXPECT_EQ(trades[0].quantity, 50);
    EXPECT_EQ(trades[1].buyOrderId, 105);
    EXPECT_EQ(trades[1].sellOrderId, 103);
    EXPECT_EQ(trades[1].quantity, 100);
    EXPECT_EQ(trades[2].buyOrderId, 105);
    EXPECT_EQ(trades[2].sellOrderId, 104);
    EXPECT_EQ(trades[2].quantity, 50);
    EXPECT_EQ(trades[3].buyOrderId, 106);
    EXPECT_EQ(trades[3].sellOrderId, 104);
    EXPECT_EQ(trades[3].quantity, 30);
}

TEST_F(PriceTimePriorityTest, AggressiveSweepOrder) {
    matchingEngine.processOrder({1.0, 201, 50000, 100, EventType::New, Side::Sell}, trades);
    matchingEngine.processOrder({1.1, 202, 50010, 150, EventType::New, Side::Sell}, trades);
    matchingEngine.processOrder({1.2, 203, 50020, 200, EventType::New, Side::Sell}, trades);
    matchingEngine.processOrder({1.3, 204, 50030, 300, EventType::New, Side::Sell}, trades);
    verifyTop(50000, 100, 0, 0);

    // Modify passive liquidity
    matchingEngine.processOrder({1.4, 202, 50010, 50, EventType::PartialCancel, Side::Sell},
                                trades);
    matchingEngine.processOrder({1.5, 203, 50020, 200, EventType::FullCancel, Side::Sell}, trades);
    verifyTop(50000, 100, 0, 0);

    // Large bid sweep
    matchingEngine.processOrder({1.6, 205, 50050, 400, EventType::New, Side::Buy}, trades);
    verifyTop(50030, 100, 0, 0);

    ASSERT_EQ(trades.size(), 3);
    EXPECT_EQ(trades[0].sellOrderId, 201);
    EXPECT_EQ(trades[0].quantity, 100);
    EXPECT_EQ(trades[1].sellOrderId, 202);
    EXPECT_EQ(trades[1].quantity, 100);
    EXPECT_EQ(trades[2].sellOrderId, 204);
    EXPECT_EQ(trades[2].quantity, 200);
}

TEST_F(PriceTimePriorityTest, ComplexExecution) {
    matchingEngine.processOrder({1.0, 301, 15000, 500, EventType::New, Side::Buy}, trades);
    matchingEngine.processOrder({1.1, 302, 15100, 300, EventType::New, Side::Buy}, trades);
    verifyTop(0, 0, 15100, 300);

    // Ask outside spread
    matchingEngine.processOrder({1.2, 303, 15200, 200, EventType::New, Side::Sell}, trades);
    verifyTop(15200, 200, 15100, 300);

    // Consuming ask
    matchingEngine.processOrder({1.3, 304, 15100, 400, EventType::New, Side::Sell}, trades);
    verifyTop(15100, 100, 15000, 500);

    // Resting bid
    matchingEngine.processOrder({1.4, 305, 15050, 100, EventType::New, Side::Buy}, trades);
    verifyTop(15100, 100, 15050, 100);

    // Aggressive bid sweep
    matchingEngine.processOrder({1.5, 306, 15200, 600, EventType::New, Side::Buy}, trades);
    verifyTop(0, 0, 15200, 300);

    ASSERT_EQ(trades.size(), 3);
    EXPECT_EQ(trades[0].buyOrderId, 302);
    EXPECT_EQ(trades[0].sellOrderId, 304);
    EXPECT_EQ(trades[0].quantity, 300);
    EXPECT_EQ(trades[1].buyOrderId, 306);
    EXPECT_EQ(trades[1].sellOrderId, 304);
    EXPECT_EQ(trades[1].quantity, 100);
    EXPECT_EQ(trades[2].buyOrderId, 306);
    EXPECT_EQ(trades[2].sellOrderId, 303);
    EXPECT_EQ(trades[2].quantity, 200);
}

TEST_F(PriceTimePriorityTest, TimePriorityWithinLevel) {
    // Bids with same price
    matchingEngine.processOrder({1.0, 401, 10000, 100, EventType::New, Side::Buy}, trades);
    matchingEngine.processOrder({1.1, 402, 10000, 100, EventType::New, Side::Buy}, trades);
    verifyTop(0, 0, 10000, 200);

    // Aggressive ask
    matchingEngine.processOrder({1.2, 403, 10000, 150, EventType::New, Side::Sell}, trades);
    verifyTop(0, 0, 10000, 50);

    ASSERT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[0].buyOrderId, 401);
    EXPECT_EQ(trades[0].quantity, 100);
    EXPECT_EQ(trades[1].buyOrderId, 402);
    EXPECT_EQ(trades[1].quantity, 50);
}

TEST_F(PriceTimePriorityTest, PriceSlippage) {
    // Passive ask
    matchingEngine.processOrder({1.0, 501, 10000, 100, EventType::New, Side::Sell}, trades);
    verifyTop(10000, 100, 0, 0);

    // Aggressive bid with too generous price
    matchingEngine.processOrder({1.1, 502, 10500, 100, EventType::New, Side::Buy}, trades);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].price, 10000);
}

TEST_F(PriceTimePriorityTest, FullSweepAndRest) {
    // Passive ask
    matchingEngine.processOrder({1.0, 601, 10000, 100, EventType::New, Side::Sell}, trades);
    verifyTop(10000, 100, 0, 0);

    // Aggressive bid consumes 100 and rests 50
    matchingEngine.processOrder({1.1, 602, 10100, 150, EventType::New, Side::Buy}, trades);
    verifyTop(0, 0, 10100, 50);

    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 100);

    // Aggressive ask to match rest
    matchingEngine.processOrder({1.2, 603, 10100, 50, EventType::New, Side::Sell}, trades);

    ASSERT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[1].buyOrderId, 602);
    EXPECT_EQ(trades[1].sellOrderId, 603);
    EXPECT_EQ(trades[1].quantity, 50);
}

TEST_F(PriceTimePriorityTest, CancelNonExistentOrAlreadyFilled) {
    // New bid
    matchingEngine.processOrder({1.0, 701, 10000, 100, EventType::New, Side::Buy}, trades);
    verifyTop(0, 0, 10000, 100);

    // Full match
    matchingEngine.processOrder({1.1, 702, 10000, 100, EventType::New, Side::Sell}, trades);

    // Try to cancel already matched order
    EXPECT_NO_THROW(
        matchingEngine.processOrder({1.2, 701, 10000, 100, EventType::FullCancel, Side::Buy}, trades
        ));
    // Try to cancel a completely random ID
    EXPECT_NO_THROW(
        matchingEngine.processOrder({1.3, 999, 50000, 100, EventType::FullCancel, Side::Sell},
            trades));

    ASSERT_EQ(trades.size(), 1);
}

} // namespace engine
