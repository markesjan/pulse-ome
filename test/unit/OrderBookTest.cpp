#include <gtest/gtest.h>

#include "OrderBook.hpp"

namespace engine {

class OrderBookTest : public testing::Test {
protected:
    OrderBook orderBook{10'000, 20'000};

    static Order createOrder(OrderId id, Price price, Quantity qty) {
        return Order{1'000'000'000, id, price, qty, 0};
    }
};

TEST_F(OrderBookTest, BestBidAskEmptyBookReturnsNullptr) {
    EXPECT_EQ(orderBook.bestBid(), nullptr);
    EXPECT_EQ(orderBook.bestAsk(), nullptr);
}

TEST_F(OrderBookTest, InsertOnceRetrievesAsBestBid) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 1);
    EXPECT_EQ(bestBid->price, 10000);
    EXPECT_EQ(bestBid->quantity, 50);
}

TEST_F(OrderBookTest, InsertOnceRetrievesAsBestAsk) {
    orderBook.insert(createOrder(1, 15000, 100), Side::Sell);

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 1);
    EXPECT_EQ(bestAsk->price, 15000);
    EXPECT_EQ(bestAsk->quantity, 100);
}

TEST_F(OrderBookTest, CancelTopBidExposesNextOrderAndPreservesLevel) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    orderBook.insert(createOrder(2, 10000, 40), Side::Buy);

    orderBook.cancelFull(1);

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 2);
    EXPECT_EQ(bestBid->quantity, 40);
}

TEST_F(OrderBookTest, CancelTopAskExposesNextOrderAndPreservesLevel) {
    orderBook.insert(createOrder(1, 15000, 50), Side::Sell);
    orderBook.insert(createOrder(2, 15000, 40), Side::Sell);

    orderBook.cancelFull(1);

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 2);
    EXPECT_EQ(bestAsk->quantity, 40);
}

TEST_F(OrderBookTest, CancelFullRemovesBidOrder) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    orderBook.cancelFull(1);

    EXPECT_EQ(orderBook.bestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelFullRemovesAskOrder) {
    orderBook.insert(createOrder(1, 15000, 50), Side::Sell);
    orderBook.cancelFull(1);

    EXPECT_EQ(orderBook.bestAsk(), nullptr);
}

TEST_F(OrderBookTest, CancelPartialReducesBidQuantity) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    orderBook.cancelPartial(1, 20);

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->quantity, 30);
}

TEST_F(OrderBookTest, CancelPartialReducesAskQuantity) {
    orderBook.insert(createOrder(1, 15000, 50), Side::Sell);
    orderBook.cancelPartial(1, 20);

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->quantity, 30);
}

TEST_F(OrderBookTest, CancelPartialGreaterOrEqualRemovesBidOrder) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    orderBook.cancelPartial(1, 50);

    EXPECT_EQ(orderBook.bestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelNonExistentBidOrderDoesNothing) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    EXPECT_NO_THROW(orderBook.cancelFull(999));

    ASSERT_NE(orderBook.bestBid(), nullptr);
    EXPECT_EQ(orderBook.bestBid()->quantity, 50);
}

TEST_F(OrderBookTest, CancelFirstOrderDoesNotCorruptIndicesOfRemainingOrders) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    orderBook.insert(createOrder(2, 10000, 40), Side::Buy);

    orderBook.cancelFull(1);

    EXPECT_NO_THROW(orderBook.cancelFull(2));

    EXPECT_EQ(orderBook.bestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelMiddleOrderDoesNotCorruptSubsequentOrders) {
    orderBook.insert(createOrder(1, 15000, 10), Side::Sell);
    orderBook.insert(createOrder(2, 15000, 20), Side::Sell);
    orderBook.insert(createOrder(3, 15000, 30), Side::Sell);

    orderBook.cancelFull(2);

    EXPECT_NO_THROW(orderBook.cancelFull(3));

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 1);
    EXPECT_EQ(bestAsk->quantity, 10);
}

TEST_F(OrderBookTest, CancelOnEmptyBookDoesNothing) {
    EXPECT_NO_THROW(orderBook.cancelFull(1));
    EXPECT_NO_THROW(orderBook.cancelFull(2));
}

TEST_F(OrderBookTest, EraseLocationRemovesIndexButLeavesPhysicalOrder) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);

    orderBook.eraseLocation(1);

    EXPECT_NO_THROW(orderBook.cancelFull(1));

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 1);
    EXPECT_EQ(bestBid->quantity, 50);
}

TEST_F(OrderBookTest, BookStateZeroDepthReturnsEmptyVector) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    auto state = orderBook.bookState(0);
    EXPECT_TRUE(state.empty());
}

TEST_F(OrderBookTest, BookStateReturnsCorrectAggregatedDepths) {
    orderBook.insert(createOrder(1, 10000, 50), Side::Buy);
    orderBook.insert(createOrder(2, 10000, 30), Side::Buy);
    orderBook.insert(createOrder(3, 9900, 100), Side::Buy);

    orderBook.insert(createOrder(4, 10100, 40), Side::Sell);
    orderBook.insert(createOrder(5, 10200, 60), Side::Sell);

    auto state = orderBook.bookState(3);

    ASSERT_EQ(state.size(), 3);

    EXPECT_EQ(state[0].bidPrice, 10000);
    EXPECT_EQ(state[0].bidQty, 80);
    EXPECT_EQ(state[0].askPrice, 10100);
    EXPECT_EQ(state[0].askQty, 40);

    EXPECT_EQ(state[1].bidPrice, 9900);
    EXPECT_EQ(state[1].bidQty, 100);
    EXPECT_EQ(state[1].askPrice, 10200);
    EXPECT_EQ(state[1].askQty, 60);

    EXPECT_EQ(state[2].bidPrice, 0);
    EXPECT_EQ(state[2].bidQty, 0);
    EXPECT_EQ(state[2].askPrice, 0);
    EXPECT_EQ(state[2].askQty, 0);
}

} // namespace engine
