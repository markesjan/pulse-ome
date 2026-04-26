#include <gtest/gtest.h>

#include "OrderBook.hpp"

namespace engine {

class OrderBookTest : public testing::Test {
protected:
    OrderBook orderBook;

    static Order createBuy(OrderId id, Price price, Quantity qty) {
        return Order{1.0, id, price, qty, EventType::New, Side::Buy};
    }

    static Order createSell(OrderId id, Price price, Quantity qty) {
        return Order{1.0, id, price, qty, EventType::New, Side::Sell};
    }

    static Order createCancel(OrderId id, Quantity qty, EventType type, Side side) {
        return Order{1.0, id, 0, qty, type, side};
    }
};

TEST_F(OrderBookTest, GetBestBidAskEmptyBookReturnsNullptr) {
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getBestAsk(), nullptr);
}

TEST_F(OrderBookTest, AddBidOnceRetrievesAsBestBid) {
    orderBook.addBid(createBuy(1, 10000, 50));

    auto bestBid = orderBook.getBestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 1);
    EXPECT_EQ(bestBid->price, 10000);
    EXPECT_EQ(bestBid->quantity, 50);
}

TEST_F(OrderBookTest, AddAskOnceRetrievesAsBestAsk) {
    orderBook.addAsk(createSell(1, 15000, 100));

    auto bestAsk = orderBook.getBestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 1);
    EXPECT_EQ(bestAsk->price, 15000);
    EXPECT_EQ(bestAsk->quantity, 100);
}

TEST_F(OrderBookTest, PopBestBidRemovesTopOrderAndPreservesLevel) {
    orderBook.addBid(createBuy(1, 10000, 50));
    orderBook.addBid(createBuy(2, 10000, 40));

    orderBook.popBestBid();

    auto bestBid = orderBook.getBestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->orderId, 2);
    EXPECT_EQ(bestBid->quantity, 40);
}

TEST_F(OrderBookTest, PopBestBidRemovesLevelWhenEmpty) {
    orderBook.addBid(createBuy(1, 10000, 50));
    orderBook.popBestBid();

    EXPECT_EQ(orderBook.getBestBid(), nullptr);
}

TEST_F(OrderBookTest, PopBestAskRemovesTopOrderAndPreservesLevel) {
    orderBook.addAsk(createSell(1, 15000, 50));
    orderBook.addAsk(createSell(2, 15000, 40));

    orderBook.popBestAsk();

    auto bestAsk = orderBook.getBestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->orderId, 2);
    EXPECT_EQ(bestAsk->quantity, 40);
}

TEST_F(OrderBookTest, PopBestAskRemovesLevelWhenEmpty) {
    orderBook.addAsk(createSell(1, 15000, 50));
    orderBook.popBestAsk();

    EXPECT_EQ(orderBook.getBestAsk(), nullptr);
}

TEST_F(OrderBookTest, PopOnEmptyBookDoesNothing) {
    EXPECT_NO_THROW(orderBook.popBestBid());
    EXPECT_NO_THROW(orderBook.popBestAsk());
}

TEST_F(OrderBookTest, CancelBuyOrderFullRemovesOrder) {
    orderBook.addBid(createBuy(1, 10000, 50));
    orderBook.cancelBuyOrder(createCancel(1, 0, EventType::FullCancel, Side::Buy));

    EXPECT_EQ(orderBook.getBestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelSellOrderFullRemovesOrder) {
    orderBook.addAsk(createSell(1, 15000, 50));
    orderBook.cancelSellOrder(createCancel(1, 0, EventType::FullCancel, Side::Sell));

    EXPECT_EQ(orderBook.getBestAsk(), nullptr);
}

TEST_F(OrderBookTest, CancelBuyOrderPartialReducesQuantity) {
    orderBook.addBid(createBuy(1, 10000, 50));
    orderBook.cancelBuyOrder(createCancel(1, 20, EventType::PartialCancel, Side::Buy));

    auto bestBid = orderBook.getBestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(bestBid->quantity, 30);
}

TEST_F(OrderBookTest, CancelSellOrderPartialReducesQuantity) {
    orderBook.addAsk(createSell(1, 15000, 50));
    orderBook.cancelSellOrder(createCancel(1, 20, EventType::PartialCancel, Side::Sell));

    auto bestAsk = orderBook.getBestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(bestAsk->quantity, 30);
}

TEST_F(OrderBookTest, CancelBuyOrderPartialGreaterOrEqualRemovesOrder) {
    orderBook.addBid(createBuy(1, 10000, 50));
    orderBook.cancelBuyOrder(createCancel(1, 50, EventType::PartialCancel, Side::Buy));

    EXPECT_EQ(orderBook.getBestBid(), nullptr);
}

TEST_F(OrderBookTest, CancelNonExistentOrderDoesNothing) {
    orderBook.addBid(createBuy(1, 10000, 50));
    EXPECT_NO_THROW(
        orderBook.cancelBuyOrder(createCancel(999, 0, EventType::FullCancel, Side::Buy)));

    ASSERT_NE(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getBestBid()->quantity, 50);
}

TEST_F(OrderBookTest, GetBookStateZeroDepthReturnsEmptyVector) {
    orderBook.addBid(createBuy(1, 10000, 50));
    auto state = orderBook.getBookState(0);
    EXPECT_TRUE(state.empty());
}

TEST_F(OrderBookTest, GetBookStateReturnsCorrectAggregatedDepths) {
    orderBook.addBid(createBuy(1, 10000, 50));
    orderBook.addBid(createBuy(2, 10000, 30));
    orderBook.addBid(createBuy(3, 9900, 100));

    orderBook.addAsk(createSell(4, 10100, 40));
    orderBook.addAsk(createSell(5, 10200, 60));

    auto state = orderBook.getBookState(3);

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
