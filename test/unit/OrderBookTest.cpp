#include <gtest/gtest.h>

#include "OrderBook.hpp"

namespace engine {

class OrderBookTest : public testing::Test {
protected:
    OrderBook orderBook;

    static Order createBuy(OrderId id, Price price, Quantity qty) {
        return Order{1'000'000'000, id, price, qty, EventType::New, Side::Buy};
    }

    static Order createSell(OrderId id, Price price, Quantity qty) {
        return Order{1'000'000'000, id, price, qty, EventType::New, Side::Sell};
    }

    static Order createCancel(OrderId id, Quantity qty, EventType type, Side side) {
        return Order{1'000'000'000, id, 0, qty, type, side};
    }
};

TEST_F(OrderBookTest, BestBidAskEmptyBookReturnsNullptr) {
    EXPECT_EQ(orderBook.bestBid(), nullptr);
    EXPECT_EQ(orderBook.bestAsk(), nullptr);
}

TEST_F(OrderBookTest, InsertOnceRetrievesAsBestBid) {
    orderBook.insert(createBuy(1, 10000, 50));

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 1);
    EXPECT_EQ(bestBid->price, 10000);
    EXPECT_EQ(bestBid->quantity, 50);
}

TEST_F(OrderBookTest, InsertOnceRetrievesAsBestAsk) {
    orderBook.insert(createSell(1, 15000, 100));

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 1);
    EXPECT_EQ(bestAsk->price, 15000);
    EXPECT_EQ(bestAsk->quantity, 100);
}

TEST_F(OrderBookTest, RemoveBestBidRemovesTopOrderAndPreservesLevel) {
    orderBook.insert(createBuy(1, 10000, 50));
    orderBook.insert(createBuy(2, 10000, 40));

    orderBook.removeBestBid();

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 2);
    EXPECT_EQ(bestBid->quantity, 40);
}

TEST_F(OrderBookTest, RemoveBestBidRemovesLevelWhenEmpty) {
    orderBook.insert(createBuy(1, 10000, 50));
    orderBook.removeBestBid();

    EXPECT_EQ(orderBook.bestBid(), nullptr);
}

TEST_F(OrderBookTest, RemoveBestAskRemovesTopOrderAndPreservesLevel) {
    orderBook.insert(createSell(1, 15000, 50));
    orderBook.insert(createSell(2, 15000, 40));

    orderBook.removeBestAsk();

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 2);
    EXPECT_EQ(bestAsk->quantity, 40);
}

TEST_F(OrderBookTest, RemoveBestAskRemovesLevelWhenEmpty) {
    orderBook.insert(createSell(1, 15000, 50));
    orderBook.removeBestAsk();

    EXPECT_EQ(orderBook.bestAsk(), nullptr);
}

TEST_F(OrderBookTest, RemoveOnEmptyBookDoesNothing) {
    EXPECT_NO_THROW(orderBook.removeBestBid());
    EXPECT_NO_THROW(orderBook.removeBestAsk());
}

TEST_F(OrderBookTest, CancelFullRemovesBidOrder) {
    orderBook.insert(createBuy(1, 10000, 50));
    orderBook.cancel(createCancel(1, 0, EventType::FullCancel, Side::Buy));

    EXPECT_EQ(orderBook.bestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelFullRemovesAskOrder) {
    orderBook.insert(createSell(1, 15000, 50));
    orderBook.cancel(createCancel(1, 0, EventType::FullCancel, Side::Sell));

    EXPECT_EQ(orderBook.bestAsk(), nullptr);
}

TEST_F(OrderBookTest, CancelPartialReducesBidQuantity) {
    orderBook.insert(createBuy(1, 10000, 50));
    orderBook.cancel(createCancel(1, 20, EventType::PartialCancel, Side::Buy));

    auto bestBid = orderBook.bestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->quantity, 30);
}

TEST_F(OrderBookTest, CancelPartialReducesAskQuantity) {
    orderBook.insert(createSell(1, 15000, 50));
    orderBook.cancel(createCancel(1, 20, EventType::PartialCancel, Side::Sell));

    auto bestAsk = orderBook.bestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->quantity, 30);
}

TEST_F(OrderBookTest, CancelPartialGreaterOrEqualRemovesBidOrder) {
    orderBook.insert(createBuy(1, 10000, 50));
    orderBook.cancel(createCancel(1, 50, EventType::PartialCancel, Side::Buy));

    EXPECT_EQ(orderBook.bestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelNonExistentBidOrderDoesNothing) {
    orderBook.insert(createBuy(1, 10000, 50));
    EXPECT_NO_THROW(
        orderBook.cancel(createCancel(999, 0, EventType::FullCancel, Side::Buy)));

    ASSERT_NE(orderBook.bestBid(), nullptr);
    EXPECT_EQ(orderBook.bestBid()->quantity, 50);
}

TEST_F(OrderBookTest, GetBookStateZeroDepthReturnsEmptyVector) {
    orderBook.insert(createBuy(1, 10000, 50));
    auto state = orderBook.bookState(0);
    EXPECT_TRUE(state.empty());
}

TEST_F(OrderBookTest, GetBookStateReturnsCorrectAggregatedDepths) {
    orderBook.insert(createBuy(1, 10000, 50));
    orderBook.insert(createBuy(2, 10000, 30));
    orderBook.insert(createBuy(3, 9900, 100));

    orderBook.insert(createSell(4, 10100, 40));
    orderBook.insert(createSell(5, 10200, 60));

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
