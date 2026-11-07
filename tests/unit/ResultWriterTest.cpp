/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  ResultWriterTest.cpp
 * @brief Unit tests for the ResultWriter class.
 */

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>

#include "ResultWriter.hpp"

namespace engine {

class ResultWriterTest : public testing::Test {
protected:
    const std::string testFilePath = "test_trades_output.csv";

    void SetUp() override {
        std::remove(testFilePath.c_str());
    }

    void TearDown() override {
        std::remove(testFilePath.c_str());
    }

    static std::vector<std::string> readLines(const std::string& path) {
        std::vector<std::string> lines;
        std::ifstream in(path);
        std::string line;

        while (std::getline(in, line))
            lines.push_back(line);

        return lines;
    }
};

TEST_F(ResultWriterTest, WriteTradesValidDataCreatesValidCsv) {
    std::vector<Trade> trades = {
        {1'000'000'000, 1001, 10, 20, 15000, 50},
        {1'500'000'000, 1002, 11, 21, 15100, 100}
    };

    ResultWriter::writeTrades(trades, testFilePath);

    auto lines = readLines(testFilePath);
    ASSERT_GE(lines.size(), 3);

    EXPECT_EQ(lines[0], "Timestamp,TradeID,BuyOrderID,SellOrderID,Quantity,Price");
    EXPECT_EQ(lines[1], "1000000000,1001,10,20,50,15000");
    EXPECT_EQ(lines[2], "1500000000,1002,11,21,100,15100");
}

TEST_F(ResultWriterTest, WriteTradesInvalidPathReturnsError) {
    testing::internal::CaptureStderr();

    ResultWriter::writeTrades({}, "/invalid_directory/test.csv");

    std::string errOutput = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(errOutput.find("Couldn't open output file") != std::string::npos);
}

TEST_F(ResultWriterTest, PrintSummaryWithLatenciesShowsLatencyMessage) {
    std::vector<uint64_t> latencies = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

    testing::internal::CaptureStdout();

    ResultWriter::printSummary(15.5, 2.5 * 1'000'000, 1000, 500, latencies);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Processed orders:    1000") != std::string::npos);
    EXPECT_TRUE(output.find("Created trades:      500") != std::string::npos);
    EXPECT_TRUE(output.find("Match rate:          50.00 %") != std::string::npos);
    EXPECT_TRUE(output.find("Avg duration:        15.50 ms") != std::string::npos);
    EXPECT_TRUE(output.find("Avg throughput:      2.50 M orders/s") != std::string::npos);
    EXPECT_TRUE(output.find("Min:                 100 ns") != std::string::npos);
    EXPECT_TRUE(output.find("Max:                 1000 ns") != std::string::npos);
    EXPECT_TRUE(output.find("Average:             550 ns") != std::string::npos);
}

TEST_F(ResultWriterTest, PrintSummaryWithoutLatenciesShowsDisabledMessage) {
    testing::internal::CaptureStdout();

    ResultWriter::printSummary(10.0, 1.0 * 1'000'000, 100, 10, {});

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Match rate:          10.00 %") != std::string::npos);
    EXPECT_TRUE(output.find("Latency measurement disabled") != std::string::npos);
}

TEST_F(ResultWriterTest, PrintSummaryZeroOrdersAvoidsDivisionByZero) {
    testing::internal::CaptureStdout();

    ResultWriter::printSummary(0.0, 0.0, 0, 0, {});

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Processed orders:    0") != std::string::npos);
    EXPECT_FALSE(output.find("Match rate:") != std::string::npos);
}

} // namespace engine
