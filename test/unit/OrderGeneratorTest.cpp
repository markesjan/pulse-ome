#include <gtest/gtest.h>

#include "OrderGenerator.hpp"

namespace engine {

TEST(OrderGeneratorTest, GenerateZeroCountReturnsEmpty) {
    auto orders = OrderGenerator::generate(42, 0, 0, 10'000, 14.0, 8);
    EXPECT_TRUE(orders.empty());
}

TEST(OrderGeneratorTest, GenerateSpecificCountReturnsExactSize) {
    auto orders = OrderGenerator::generate(42, 1000, 10, 10'000, 14.0, 8);
    EXPECT_EQ(orders.size(), 1000);
}

TEST(OrderGeneratorTest, GenerateSameSeedProducesIdenticalResults) {
    auto orders1 = OrderGenerator::generate(12345, 500, 20, 10'000, 14.0, 8);
    auto orders2 = OrderGenerator::generate(12345, 500, 20, 10'000, 14.0, 8);

    ASSERT_EQ(orders1.size(), orders2.size());
    for (std::size_t i = 0; i < orders1.size(); ++i) {
        EXPECT_EQ(orders1[i].timestamp, orders2[i].timestamp);
        EXPECT_EQ(orders1[i].orderId, orders2[i].orderId);
        EXPECT_EQ(orders1[i].price, orders2[i].price);
        EXPECT_EQ(orders1[i].quantity, orders2[i].quantity);
        EXPECT_EQ(orders1[i].eventType, orders2[i].eventType);
        EXPECT_EQ(orders1[i].side, orders2[i].side);
    }
}

TEST(OrderGeneratorTest, GenerateDifferentSeedProducesDifferentResults) {
    auto orders1 = OrderGenerator::generate(1, 100, 0, 10'000, 14.0, 8);
    auto orders2 = OrderGenerator::generate(2, 100, 0, 10'000, 14.0, 8);

    ASSERT_EQ(orders1.size(), orders2.size());

    bool hasDifference = false;
    for (std::size_t i = 0; i < orders1.size(); ++i) {
        if (orders1[i].price != orders2[i].price || orders1[i].side != orders2[i].side) {
            hasDifference = true;
            break;
        }
    }
    EXPECT_TRUE(hasDifference);
}

TEST(OrderGeneratorTest, GenerateZeroCancelReturnsOnlyNewOrders) {
    auto orders = OrderGenerator::generate(42, 500, 0, 10'000, 14.0, 8);

    for (const auto& order : orders) {
        EXPECT_EQ(order.eventType, EventType::New);
        EXPECT_GT(order.price, 0);
        EXPECT_GT(order.quantity, 0);
    }
}

TEST(OrderGeneratorTest, GenerateWithCancelsReturnsBothCancelTypes) {
    auto orders = OrderGenerator::generate(42, 1000, 50, 10'000, 14.0, 8);

    bool hasCancel = false;
    for (const auto& order : orders) {
        if (order.eventType == EventType::FullCancel) {
            EXPECT_EQ(order.quantity, 0);
            EXPECT_EQ(order.price, 0);
            hasCancel = true;
        } else if (order.eventType == EventType::PartialCancel) {
            EXPECT_GT(order.quantity, 0);
            EXPECT_LE(order.quantity, 100);
            EXPECT_EQ(order.price, 0);
            hasCancel = true;
        }
    }
    EXPECT_TRUE(hasCancel);
}

TEST(OrderGeneratorTest, GenerateTimestampsAreStrictlyIncreasing) {
    auto orders = OrderGenerator::generate(42, 100, 10, 10'000, 14.0, 8);

    ASSERT_FALSE(orders.empty());
    Timestamp prevTimestamp = orders[0].timestamp;

    for (std::size_t i = 1; i < orders.size(); ++i) {
        EXPECT_GT(orders[i].timestamp, prevTimestamp);
        prevTimestamp = orders[i].timestamp;
    }
}

TEST(OrderGeneratorTest, GeneratePricesNeverDropBelowOne) {
    auto orders = OrderGenerator::generate(42, 1000, 0, 5, 14.0, 8.0);

    for (const auto& order : orders) {
        EXPECT_GE(order.price, 1);
    }
}

} // namespace engine
