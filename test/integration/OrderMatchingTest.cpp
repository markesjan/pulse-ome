#include <gtest/gtest.h>
#include "../../include/DataLoader.hpp"
#include "../../include/MatchingEngine.hpp"

TEST(OrderMatchingTest, Scenario1) {
    auto orders = engine::DataLoader::loadOrders("../test/integration/data/scenario1.csv");
    ASSERT_FALSE(orders.empty());

    engine::MatchingEngine matchEngine;
    std::vector<engine::Trade> allTrades;

    for (const auto& order : orders) {
        auto newTrades = matchEngine.processOrder(order);
        allTrades.insert(allTrades.end(), newTrades.begin(), newTrades.end());
    }

    ASSERT_EQ(allTrades.size(), 4);

    EXPECT_EQ(allTrades[0].buyOrderId, 101);
    EXPECT_EQ(allTrades[0].sellOrderId, 102);
    EXPECT_EQ(allTrades[0].price, 10000);
    EXPECT_EQ(allTrades[0].quantity, 50);

    EXPECT_EQ(allTrades[1].sellOrderId, 103);
    EXPECT_EQ(allTrades[1].buyOrderId, 105);
    EXPECT_EQ(allTrades[1].price, 10100);
    EXPECT_EQ(allTrades[1].quantity, 100);

    EXPECT_EQ(allTrades[2].sellOrderId, 104);
    EXPECT_EQ(allTrades[2].buyOrderId, 105);
    EXPECT_EQ(allTrades[2].price, 10200);
    EXPECT_EQ(allTrades[2].quantity, 50);

    EXPECT_EQ(allTrades[3].sellOrderId, 104);
    EXPECT_EQ(allTrades[3].buyOrderId, 106);
    EXPECT_EQ(allTrades[3].price, 10200);
    EXPECT_EQ(allTrades[3].quantity, 30);
}

TEST(OrderMatchingTest, Scenario2) {
    auto orders = engine::DataLoader::loadOrders("../test/integration/data/scenario2.csv");
    ASSERT_FALSE(orders.empty());

    engine::MatchingEngine matchEngine;
    std::vector<engine::Trade> allTrades;

    for (const auto& order : orders) {
        auto newTrades = matchEngine.processOrder(order);
        allTrades.insert(allTrades.end(), newTrades.begin(), newTrades.end());
    }

    ASSERT_EQ(allTrades.size(), 3);

    EXPECT_EQ(allTrades[0].sellOrderId, 201);
    EXPECT_EQ(allTrades[0].buyOrderId, 205);
    EXPECT_EQ(allTrades[0].price, 50000);
    EXPECT_EQ(allTrades[0].quantity, 100);

    EXPECT_EQ(allTrades[1].sellOrderId, 202);
    EXPECT_EQ(allTrades[1].buyOrderId, 205);
    EXPECT_EQ(allTrades[1].price, 50010);
    EXPECT_EQ(allTrades[1].quantity, 100);

    EXPECT_EQ(allTrades[2].sellOrderId, 204);
    EXPECT_EQ(allTrades[2].buyOrderId, 205);
    EXPECT_EQ(allTrades[2].price, 50030);
    EXPECT_EQ(allTrades[2].quantity, 200);
}

TEST(OrderMatchingTest, Scenario3) {
    auto orders = engine::DataLoader::loadOrders("../test/integration/data/scenario3.csv");
    ASSERT_FALSE(orders.empty());

    engine::MatchingEngine matchEngine;
    std::vector<engine::Trade> allTrades;

    for (const auto& order : orders) {
        auto newTrades = matchEngine.processOrder(order);
        allTrades.insert(allTrades.end(), newTrades.begin(), newTrades.end());
    }

    ASSERT_EQ(allTrades.size(), 3);

    EXPECT_EQ(allTrades[0].buyOrderId, 302);
    EXPECT_EQ(allTrades[0].sellOrderId, 304);
    EXPECT_EQ(allTrades[0].price, 15100);
    EXPECT_EQ(allTrades[0].quantity, 300);

    EXPECT_EQ(allTrades[1].sellOrderId, 304);
    EXPECT_EQ(allTrades[1].buyOrderId, 306);
    EXPECT_EQ(allTrades[1].price, 15100);
    EXPECT_EQ(allTrades[1].quantity, 100);

    EXPECT_EQ(allTrades[2].sellOrderId, 303);
    EXPECT_EQ(allTrades[2].buyOrderId, 306);
    EXPECT_EQ(allTrades[2].price, 15200);
    EXPECT_EQ(allTrades[2].quantity, 200);
}
