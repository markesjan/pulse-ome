#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "ConfigParser.hpp"

namespace engine {

class ConfigParserTest : public testing::Test {
protected:
    void SetUp() override {
        testing::internal::CaptureStdout();
    }

    void TearDown() override {
        testing::internal::GetCapturedStdout();
    }

    static Config parse(const std::vector<std::string>& args) {
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>("./matching_engine"));

        for (const auto& arg : args)
            argv.push_back(const_cast<char*>(arg.c_str()));

        return ConfigParser::parseArguments(static_cast<int>(argv.size()), argv.data());
    }
};

TEST_F(ConfigParserTest, ParseArgumentsDefaultValuesCreatesDefaultConfig) {
    auto config = parse({});

    EXPECT_EQ(config.orderFile, "");
    EXPECT_EQ(config.tradeFile, "");
    EXPECT_EQ(config.generateCnt, 100'000);
    EXPECT_FALSE(config.measureLatency);
}

TEST_F(ConfigParserTest, ParseArgumentsShortFlagsCreatesValidConfig) {
    auto config = parse({
        "-i", "../input.csv",
        "-o", "../out/output.csv",
        "-g", "1000",
        "-s", "67",
        "-l"});

    EXPECT_EQ(config.orderFile, "../input.csv");
    EXPECT_EQ(config.tradeFile, "../out/output.csv");
    EXPECT_EQ(config.generateCnt, 1000);
    EXPECT_EQ(config.seed, 67);
    EXPECT_TRUE(config.measureLatency);
}

TEST_F(ConfigParserTest, ParseArgumentsLongFlagsCreatesValidConfig) {
    auto config = parse({
        "--input", "data_in.csv",
        "--output", "data_out.csv",
        "--generate", "5000",
        "--seed", "99",
        "--latency"});

    EXPECT_EQ(config.orderFile, "data_in.csv");
    EXPECT_EQ(config.tradeFile, "data_out.csv");
    EXPECT_EQ(config.generateCnt, 5000);
    EXPECT_EQ(config.seed, 99);
    EXPECT_TRUE(config.measureLatency);
}

TEST_F(ConfigParserTest, ParseArgumentsInvalidArgumentExitsWith1) {
    EXPECT_EXIT(parse({"--unknown-flag"}),
                testing::ExitedWithCode(1),
                "Invalid argument: --unknown-flag");
}

TEST_F(ConfigParserTest, ParseArgumentsHelpFlagExitsWith0) {
    EXPECT_EXIT(parse({"--help"}),
                testing::ExitedWithCode(0),
                "");
}

TEST_F(ConfigParserTest, ParseArgumentsMissingValueForFlagExitsWith1) {
    EXPECT_EXIT(parse({"-i"}),
                testing::ExitedWithCode(1),
                "Invalid argument: -i");
}

} // namespace engine
