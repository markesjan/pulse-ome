#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <string>

#include "LobsterOrderLoader.hpp"

namespace engine {

class LobsterOrderLoaderTest : public testing::Test {
protected:
    const std::string testFilePath = "test_orders.csv";

    void SetUp() override {
        std::remove(testFilePath.c_str());

        testing::internal::CaptureStdout();
    }

    void TearDown() override {
        std::remove(testFilePath.c_str());

        testing::internal::GetCapturedStdout();
    }

    void createDummyCsv(const std::string& content) const {
        std::ofstream out(testFilePath);
        out << content;
        out.close();
    }
};

TEST_F(LobsterOrderLoaderTest, LoadValidCsvReturnsOrders) {
    std::string csvData =
        "34200.03988397,1,16135101,300,309500,1\n"
        "34200.12345678,4,16135102,100,309500,1\n"
        "34200.203283207,3,16166153,100,309700,-1\n";

    createDummyCsv(csvData);
    auto orders = LobsterOrderLoader::load(testFilePath);

    ASSERT_EQ(orders.size(), 2);

    EXPECT_DOUBLE_EQ(orders[0].timestamp, 34200.03988397);
    EXPECT_EQ(orders[0].eventType, EventType::New);
    EXPECT_EQ(orders[0].orderId, 16135101);
    EXPECT_EQ(orders[0].quantity, 300);
    EXPECT_EQ(orders[0].price, 309500);
    EXPECT_EQ(orders[0].side, Side::Buy);

    EXPECT_DOUBLE_EQ(orders[1].timestamp, 34200.203283207);
    EXPECT_EQ(orders[1].eventType, EventType::FullCancel);
    EXPECT_EQ(orders[1].orderId, 16166153);
    EXPECT_EQ(orders[1].quantity, 100);
    EXPECT_EQ(orders[1].price, 309700);
    EXPECT_EQ(orders[1].side, Side::Sell);
}

TEST_F(LobsterOrderLoaderTest, LoadEmptyFileReturnsEmpty) {
    createDummyCsv("");
    auto orders = LobsterOrderLoader::load(testFilePath);
    EXPECT_TRUE(orders.empty());
}

TEST_F(LobsterOrderLoaderTest, LoadInvalidLinesSkipsToNext) {
    std::string csvData =
        "34200.03988397,1,16135101,300,309500,1\n"
        "some,random,words\n"
        "missing_commas_and_numbers\n"
        "34200.203283207,3,16166153,100,309700,-1\n";

    createDummyCsv(csvData);
    auto orders = LobsterOrderLoader::load(testFilePath);

    EXPECT_EQ(orders.size(), 2);
}

TEST_F(LobsterOrderLoaderTest, LoadNonExistentFileExitsWith1) {
    const std::string badPath = "not_exists_12345.csv";

    EXPECT_EXIT(LobsterOrderLoader::load(badPath),
                testing::ExitedWithCode(1),
                "Couldn't open file: not_exists_12345.csv");
}

} // namespace engine
